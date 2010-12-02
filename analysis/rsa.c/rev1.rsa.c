#include <openssl/opensslconf.h>

///////////////////////////////////////
//  Begin conditional inclusion logic
///////////////////////////////////////
#ifndef OPENSSL_NO_RSA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "apps.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/bn.h>

#undef PROG
#define PROG	rsa_main

int MAIN(int, char **);

int MAIN(int argc, char **argv)
	{
	//////////////////////////////////////////////////////////////////////////////////
	//  In addition to initializing the variables, this section represents the 
	//  default argument settings.
	//////////////////////////////////////////////////////////////////////////////////
	ENGINE *e = NULL;
	int ret=1;
	RSA *rsa=NULL;
	int i,badops=0, sgckey=0;
	const EVP_CIPHER *enc=NULL;
	BIO *out=NULL;
	int informat,outformat,text=0,check=0,noout=0;
	int pubin = 0, pubout = 0;
	char *infile,*outfile,*prog;
	char *passargin = NULL, *passargout = NULL;
	char *passin = NULL, *passout = NULL;
#ifndef OPENSSL_NO_ENGINE
	char *engine=NULL;
#endif
	int modulus=0;

	int pvk_encr = 2;

	apps_startup();

	if (bio_err == NULL)
		if ((bio_err=BIO_new(BIO_s_file())) != NULL)
			BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);

	if (!load_config(bio_err, NULL))
		goto end; /* <************** GOTO END ******************> */

	infile=NULL;
	outfile=NULL;
	informat=FORMAT_PEM;
	outformat=FORMAT_PEM;

    //prog is only used in the "bad" section
	prog=argv[0];
	argc--;
	argv++;
	/////////////////////////////////////////////////////////////////////////
	//  Begin iterating through **argv, validating arguments and 
	//  updating variables to represent input values (as opposed to defaults)
	/////////////////////////////////////////////////////////////////////////
	while (argc >= 1)
		{
		//////////////////////////////////////////////////////////////
		//  This sub-section contains input which requires validation.
		//  It can be summarized: 
		//      if(current argument matches input type):
		//          if(no matching argument value has been provided)
		//              goto bad
		//          else
		//              set variable referencing argument
		//              increment argv
		//////////////////////////////////////////////////////////////
		if 	(strcmp(*argv,"-inform") == 0)
			{
			if (--argc < 1) goto bad; /* <************** GOTO BAD ******************> */
			informat=str2fmt(*(++argv));
			}
		else if (strcmp(*argv,"-outform") == 0)
			{
			if (--argc < 1) goto bad; /* <************** GOTO BAD ******************> */
			outformat=str2fmt(*(++argv));
			}
		else if (strcmp(*argv,"-in") == 0)
			{
			if (--argc < 1) goto bad; /* <************** GOTO BAD ******************> */
			infile= *(++argv);
			}
		else if (strcmp(*argv,"-out") == 0)
			{
			if (--argc < 1) goto bad; /* <************** GOTO BAD ******************> */
			outfile= *(++argv);
			}
		else if (strcmp(*argv,"-passin") == 0)
			{
			if (--argc < 1) goto bad; /* <************** GOTO BAD ******************> */
			passargin= *(++argv);
			}
		else if (strcmp(*argv,"-passout") == 0)
			{
			if (--argc < 1) goto bad; /* <************** GOTO BAD ******************> */
			passargout= *(++argv);
			}
#ifndef OPENSSL_NO_ENGINE
		else if (strcmp(*argv,"-engine") == 0)
			{
			if (--argc < 1) goto bad; /* <************** GOTO BAD ******************> */
			engine= *(++argv);
			}
#endif
        /////////////////////////////////////////////////////////////////////
        //  The following arguments do not need to be validated.
        /////////////////////////////////////////////////////////////////////
		else if (strcmp(*argv,"-sgckey") == 0)
			sgckey=1;
		else if (strcmp(*argv,"-pubin") == 0)
			pubin=1;
		else if (strcmp(*argv,"-pubout") == 0)
			pubout=1;
		else if (strcmp(*argv,"-RSAPublicKey_in") == 0)
			pubin = 2;
		else if (strcmp(*argv,"-RSAPublicKey_out") == 0)
			pubout = 2;
		else if (strcmp(*argv,"-pvk-strong") == 0)
			pvk_encr=2;
		else if (strcmp(*argv,"-pvk-weak") == 0)
			pvk_encr=1;
		else if (strcmp(*argv,"-pvk-none") == 0)
			pvk_encr=0;
		else if (strcmp(*argv,"-noout") == 0)
			noout=1;
		else if (strcmp(*argv,"-text") == 0)
			text=1;
		else if (strcmp(*argv,"-modulus") == 0)
			modulus=1;
		else if (strcmp(*argv,"-check") == 0)
			check=1;
		// This is last so that all of the above are checked first each time.
		// Intellectually, it belongs more with the other "goto bad" options.
		else if ((enc=EVP_get_cipherbyname(&(argv[0][1]))) == NULL)
			{
			BIO_printf(bio_err,"unknown option %s\n",*argv);
			badops=1;
			break; /* <************** GOTO BAD (obscured) ******************> */
			}
		argc--;
		argv++;
		}

    // badops is used only once, and that should be swapped with goto bad
    // if this method is to be internally consistent.
	if (badops) 
		{
bad:
	    /////////////////////////////////////////////////////////////////////////
	    //  Not sure why this is accessed through goto instead of using an
	    //  exterior function.  Immediate thought is that this has to do with
	    //  the desire for one in, one out?  But this is far more annoying and 
	    //  confusing.  It might have been better to simply done return bad();
	    /////////////////////////////////////////////////////////////////////////
		BIO_printf(bio_err,"%s [options] <infile >outfile\n",prog);
		BIO_printf(bio_err,"where options are\n");
		BIO_printf(bio_err," -inform arg     input format - one of DER NET PEM\n");
		BIO_printf(bio_err," -outform arg    output format - one of DER NET PEM\n");
		BIO_printf(bio_err," -in arg         input file\n");
		BIO_printf(bio_err," -sgckey         Use IIS SGC key format\n");
		BIO_printf(bio_err," -passin arg     input file pass phrase source\n");
		BIO_printf(bio_err," -out arg        output file\n");
		BIO_printf(bio_err," -passout arg    output file pass phrase source\n");
		BIO_printf(bio_err," -des            encrypt PEM output with cbc des\n");
		BIO_printf(bio_err," -des3           encrypt PEM output with ede cbc des using 168 bit key\n");
#ifndef OPENSSL_NO_IDEA
		BIO_printf(bio_err," -idea           encrypt PEM output with cbc idea\n");
#endif
#ifndef OPENSSL_NO_SEED
		BIO_printf(bio_err," -seed           encrypt PEM output with cbc seed\n");
#endif
#ifndef OPENSSL_NO_AES
		BIO_printf(bio_err," -aes128, -aes192, -aes256\n");
		BIO_printf(bio_err,"                 encrypt PEM output with cbc aes\n");
#endif
#ifndef OPENSSL_NO_CAMELLIA
		BIO_printf(bio_err," -camellia128, -camellia192, -camellia256\n");
		BIO_printf(bio_err,"                 encrypt PEM output with cbc camellia\n");
#endif
		BIO_printf(bio_err," -text           print the key in text\n");
		BIO_printf(bio_err," -noout          don't print key out\n");
		BIO_printf(bio_err," -modulus        print the RSA key modulus\n");
		BIO_printf(bio_err," -check          verify key consistency\n");
		BIO_printf(bio_err," -pubin          expect a public key in input file\n");
		BIO_printf(bio_err," -pubout         output a public key\n");
#ifndef OPENSSL_NO_ENGINE
		BIO_printf(bio_err," -engine e       use engine e, possibly a hardware device.\n");
#endif
        ///////////////////////////////////////////////////////////
        //  Since an error has occurred, exit the script
        ///////////////////////////////////////////////////////////
		goto end;/* <************** GOTO END ******************> */
		}

	ERR_load_crypto_strings();

#ifndef OPENSSL_NO_ENGINE
        e = setup_engine(bio_err, engine, 0);
#endif

    //  Check password
	if(!app_passwd(bio_err, passargin, passargout, &passin, &passout)) {
		BIO_printf(bio_err, "Error getting passwords\n");
		goto end;/* <************** GOTO END ******************> */
	}

    //  Validate key check
	if(check && pubin) {
		BIO_printf(bio_err, "Only private keys can be checked\n");
		goto end;/* <************** GOTO END ******************> */
	}

	out=BIO_new(BIO_s_file());
    //////////////////////////////////////////////////////////////////
    //
    //  Note the conditionless braces.  That means the original 
    //  coder thought that this was the most important part of the
    //  function.  A breif look below will confirm that this is where
    //  the RSA object will actually be instantiated here.
    //
    //////////////////////////////////////////////////////////////////
	{
		EVP_PKEY	*pkey;

		if (pubin)
		{
		    int tmpformat=-1;
		    if (pubin == 2)
		    {
		        if (informat == FORMAT_PEM)
			        tmpformat = FORMAT_PEMRSA;
		        else if (informat == FORMAT_ASN1)
			        tmpformat = FORMAT_ASN1RSA;
		    }
		    else if (informat == FORMAT_NETSCAPE && sgckey)
			    tmpformat = FORMAT_IISSGC;
		    else
			    tmpformat = informat;
				
		    pkey = load_pubkey(bio_err, infile, tmpformat, 1,
			    passin, e, "Public Key");
		}
		else
			pkey = load_key(bio_err, infile,
				(informat == FORMAT_NETSCAPE && sgckey ?
					FORMAT_IISSGC : informat), 1,
				passin, e, "Private Key");

        /////////////////////////////////////////////////////////////
        //  I'm really not certain why this logic is here, it would
        //  seem far better to force exit if it is null.
        /////////////////////////////////////////////////////////////
		if (pkey != NULL)
			rsa = EVP_PKEY_get1_RSA(pkey);
		EVP_PKEY_free(pkey);
	}

    // if pkey is null, rsa will be null.
	if (rsa == NULL)
	{
	    ERR_print_errors(bio_err);
	    goto end; /* <************** GOTO END ******************> */
	}

	if (outfile == NULL)
		{
		BIO_set_fp(out,stdout,BIO_NOCLOSE);
#ifdef OPENSSL_SYS_VMS
		{
		BIO *tmpbio = BIO_new(BIO_f_linebuffer());
		out = BIO_push(tmpbio, out);
		}
#endif
		}
	else
		{
		if (BIO_write_filename(out,outfile) <= 0)
			{
			perror(outfile);
			goto end; /* <************** GOTO END ******************> */
			}
		}

	if (text) 
		if (!RSA_print(out,rsa,0))
			{
			perror(outfile);
			ERR_print_errors(bio_err);
			goto end;
			}

	if (modulus)
		{
		BIO_printf(out,"Modulus=");
		BN_print(out,rsa->n);
		BIO_printf(out,"\n");
		}

	if (check)
		{
		int r = RSA_check_key(rsa);

		if (r == 1)
			BIO_printf(out,"RSA key ok\n");
		else if (r == 0)
			{
			unsigned long err;

			while ((err = ERR_peek_error()) != 0 &&
				ERR_GET_LIB(err) == ERR_LIB_RSA &&
				ERR_GET_FUNC(err) == RSA_F_RSA_CHECK_KEY &&
				ERR_GET_REASON(err) != ERR_R_MALLOC_FAILURE)
				{
				BIO_printf(out, "RSA key error: %s\n", ERR_reason_error_string(err));
				ERR_get_error(); /* remove e from error stack */
				}
			}
		
		if (r == -1 || ERR_peek_error() != 0) /* should happen only if r == -1 */
			{
			ERR_print_errors(bio_err);
			goto end; /* <************** GOTO END ******************> */
			}
		}
		
	if (noout)
		{
		ret = 0;
		goto end; /* <************** GOTO END ******************> */
		}
	BIO_printf(bio_err,"writing RSA key\n");
	if 	(outformat == FORMAT_ASN1) {
		if(pubout || pubin) 
			{
			if (pubout == 2)
				i=i2d_RSAPublicKey_bio(out,rsa);
			else
				i=i2d_RSA_PUBKEY_bio(out,rsa);
			}
		else i=i2d_RSAPrivateKey_bio(out,rsa);default
	}
#ifndef OPENSSL_NO_RC4
	else if (outformat == FORMAT_NETSCAPE)
		{
		unsigned char *p,*pp;
		int size;

		i=1;
		size=i2d_RSA_NET(rsa,NULL,NULL, sgckey);
		if ((p=(unsigned char *)OPENSSL_malloc(size)) == NULL)
			{
			BIO_printf(bio_err,"Memory allocation failure\n");
			goto end; /* <************** GOTO END ******************> */
			}
		pp=p;
		i2d_RSA_NET(rsa,&p,NULL, sgckey);
		BIO_write(out,(char *)pp,size);
		OPENSSL_free(pp);
		}
#endif
	else if (outformat == FORMAT_PEM) {
		if(pubout || pubin)
			{
			if (pubout == 2)
		    		i=PEM_write_bio_RSAPublicKey(out,rsa);
			else
		    		i=PEM_write_bio_RSA_PUBKEY(out,rsa);
			}
		else i=PEM_write_bio_RSAPrivateKey(out,rsa,
						enc,NULL,0,NULL,passout);
#if !defined(OPENSSL_NO_DSA) && !defined(OPENSSL_NO_RC4)
	} else if (outformat == FORMAT_MSBLOB || outformat == FORMAT_PVK) {
		EVP_PKEY *pk;
		pk = EVP_PKEY_new();
		EVP_PKEY_set1_RSA(pk, rsa);
		if (outformat == FORMAT_PVK)
			i = i2b_PVK_bio(out, pk, pvk_encr, 0, passout);
		else if (pubin || pubout)
			i = i2b_PublicKey_bio(out, pk);
		else
			i = i2b_PrivateKey_bio(out, pk);
		EVP_PKEY_free(pk);
#endif
	} else	{
		BIO_printf(bio_err,"bad output format specified for outfile\n");
		goto end; /* <************** GOTO END ******************> */
		}
	if (i <= 0)
		{
		BIO_printf(bio_err,"unable to write key\n");
		ERR_print_errors(bio_err);
		}
	else
		ret=0;
end:
	if(out != NULL) BIO_free_all(out);
	if(rsa != NULL) RSA_free(rsa);
	if(passin) OPENSSL_free(passin);
	if(passout) OPENSSL_free(passout);
	apps_shutdown();
	OPENSSL_EXIT(ret);
	}

///////////////////////////////////////
//  Conditional exclusion logic
///////////////////////////////////////
#else /* !OPENSSL_NO_RSA */

# if PEDANTIC
static void *dummy=&dummy;
# endif

///////////////////////////////////////
//  End conditional inclusion logic
///////////////////////////////////////
#endif
