static int convert (iconv_t cd, FILE* infile, const char* infilename)
{
  char inbuf[4096+4096];
  size_t inbufrest = 0;
  char outbuf[4096];
  int status = 0;

#if O_BINARY
  if (force_binary)
    SET_BINARY(fileno(infile));
#endif
  iconv(cd,NULL,NULL,NULL,NULL);
  for (;;) {
    size_t inbufsize = fread(inbuf+4096,1,4096,infile);
    if (inbufsize == 0) {
      if (inbufrest == 0)
        break;
      else {
        if (!silent)
          fprintf(stderr,_("iconv: %s: incomplete character or shift sequence\n"),infilename);
        return 1;
      }
    } else {
      const char* inptr = inbuf+4096-inbufrest;
      size_t insize = inbufrest+inbufsize;
      inbufrest = 0;
      while (insize > 0) {
        char* outptr = outbuf;
        size_t outsize = sizeof(outbuf);
        size_t res = iconv(cd,(ICONV_CONST char**)&inptr,&insize,&outptr,&outsize);
        if (outptr != outbuf) {
          int saved_errno = errno;
          if (fwrite(outbuf,1,outptr-outbuf,stdout) < outptr-outbuf)
            return 1;
          errno = saved_errno;
        }
        if (res == (size_t)(-1)) {
          if (errno == EILSEQ) {
            if (discard_unconvertible == 1) {
              int one = 1;
              iconvctl(cd,ICONV_SET_DISCARD_ILSEQ,&one);
              discard_unconvertible = 2;
              status = 1;
            } else {
              if (!silent)
                fprintf(stderr,_("iconv: %s: cannot convert\n"),infilename);
              return 1;
            }
          } else if (errno == EINVAL) {
            if (inbufsize == 0 || insize > 4096) {
              if (!silent)
                fprintf(stderr,_("iconv: %s: incomplete character or shift sequence\n"),infilename);
              return 1;
            } else {
              inbufrest = insize;
              if (insize > 0) {
                /* Like memcpy(inbuf+4096-insize,inptr,insize), except that
                   we cannot use memcpy here, because source and destination
                   regions may overlap. */
                char* restptr = inbuf+4096-insize;
                do { *restptr++ = *inptr++; } while (--insize > 0);
              }
              break;
            }
          } else if (errno != E2BIG) {
            if (!silent) {
              int saved_errno = errno;
              fprintf(stderr,_("iconv: %s: "),infilename);
              errno = saved_errno;
              perror("");
            }
            return 1;
          }
        }
      }
    }
  }
  {
    char* outptr = outbuf;
    size_t outsize = sizeof(outbuf);
    size_t res = iconv(cd,NULL,NULL,&outptr,&outsize);
    if (outptr != outbuf) {
      int saved_errno = errno;
      if (fwrite(outbuf,1,outptr-outbuf,stdout) < outptr-outbuf)
        return 1;
      errno = saved_errno;
    }
    if (res == (size_t)(-1)) {
      if (errno == EILSEQ) {
        if (discard_unconvertible == 1) {
          int one = 1;
          iconvctl(cd,ICONV_SET_DISCARD_ILSEQ,&one);
          discard_unconvertible = 2;
          status = 1;
        } else {
          if (!silent)
            fprintf(stderr,_("iconv: %s: cannot convert\n"),infilename);
          return 1;
        }
      } else if (errno == EINVAL) {
        if (!silent)
          fprintf(stderr,_("iconv: %s: incomplete character or shift sequence\n"),infilename);
        return 1;
      } else {
        if (!silent) {
          int saved_errno = errno;
          fprintf(stderr,_("iconv: %s: "),infilename);
          errno = saved_errno;
          perror("");
        }
        return 1;
      }
    }
  }
  if (ferror(infile)) {
    fprintf(stderr,_("iconv: %s: I/O error\n"),infilename);
    return 1;
  }
  return status;
}


