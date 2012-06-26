#!/bin/bash

function createlnk
{
	tarfile=$1
	symdir=${2%/*}
	symname=`basename $2`
	if [[ $symdir"/"$symname != $2 ]]; then
		echo 'Warning: Error symlink.' 1>&2
	fi

	if [[ ! -d $symdir ]]; then
		mkdir -p $symdir
	fi
	
	cd $symdir
	ln -s $tarfile $symname
	cd -
}


createlnk Makefile.ssl ./crypto/aes/Makefile
createlnk Makefile.ssl ./crypto/asn1/Makefile
createlnk Makefile.ssl ./crypto/bf/Makefile
createlnk Makefile.ssl ./crypto/bio/Makefile
createlnk Makefile.ssl ./crypto/bn/Makefile
createlnk Makefile.ssl ./crypto/buffer/Makefile
createlnk Makefile.ssl ./crypto/cast/Makefile
createlnk Makefile.ssl ./crypto/comp/Makefile
createlnk Makefile.ssl ./crypto/conf/Makefile
createlnk Makefile.ssl ./crypto/des/Makefile
createlnk Makefile.ssl ./crypto/dh/Makefile
createlnk Makefile.ssl ./crypto/dsa/Makefile
createlnk Makefile.ssl ./crypto/dso/Makefile
createlnk Makefile.ssl ./crypto/ec/Makefile
createlnk Makefile.ssl ./crypto/engine/Makefile
createlnk Makefile.ssl ./crypto/err/Makefile
createlnk Makefile.ssl ./crypto/evp/Makefile
createlnk Makefile.ssl ./crypto/hmac/Makefile
createlnk Makefile.ssl ./crypto/idea/Makefile
createlnk Makefile.ssl ./crypto/krb5/Makefile
createlnk Makefile.ssl ./crypto/lhash/Makefile
createlnk Makefile.ssl ./crypto/Makefile
createlnk Makefile.ssl ./crypto/md2/Makefile
createlnk Makefile.ssl ./crypto/md4/Makefile
createlnk Makefile.ssl ./crypto/md5/Makefile
createlnk Makefile.ssl ./crypto/mdc2/Makefile
createlnk Makefile.ssl ./crypto/objects/Makefile
createlnk Makefile.ssl ./crypto/ocsp/Makefile
createlnk Makefile.ssl ./crypto/pem/Makefile
createlnk Makefile.ssl ./crypto/pkcs12/Makefile
createlnk Makefile.ssl ./crypto/pkcs7/Makefile
createlnk Makefile.ssl ./crypto/rand/Makefile
createlnk Makefile.ssl ./crypto/rc2/Makefile
createlnk Makefile.ssl ./crypto/rc4/Makefile
createlnk Makefile.ssl ./crypto/rc5/Makefile
createlnk Makefile.ssl ./crypto/ripemd/Makefile
createlnk Makefile.ssl ./crypto/rsa/Makefile
createlnk Makefile.ssl ./crypto/sha/Makefile
createlnk Makefile.ssl ./crypto/stack/Makefile
createlnk Makefile.ssl ./crypto/txt_db/Makefile
createlnk Makefile.ssl ./crypto/ui/Makefile
createlnk Makefile.ssl ./crypto/x509/Makefile
createlnk Makefile.ssl ./crypto/x509v3/Makefile
createlnk ../../crypto/aes/aes.h ./include/openssl/aes.h
createlnk ../../crypto/asn1/asn1.h ./include/openssl/asn1.h
createlnk ../../crypto/asn1/asn1_mac.h ./include/openssl/asn1_mac.h
createlnk ../../crypto/asn1/asn1t.h ./include/openssl/asn1t.h
createlnk ../../crypto/bio/bio.h ./include/openssl/bio.h
createlnk ../../crypto/bf/blowfish.h ./include/openssl/blowfish.h
createlnk ../../crypto/bn/bn.h ./include/openssl/bn.h
createlnk ../../crypto/buffer/buffer.h ./include/openssl/buffer.h
createlnk ../../crypto/cast/cast.h ./include/openssl/cast.h
createlnk ../../crypto/comp/comp.h ./include/openssl/comp.h
createlnk ../../crypto/conf/conf_api.h ./include/openssl/conf_api.h
createlnk ../../crypto/conf/conf.h ./include/openssl/conf.h
createlnk ../../crypto/crypto.h ./include/openssl/crypto.h
createlnk ../../crypto/des/des.h ./include/openssl/des.h
createlnk ../../crypto/des/des_old.h ./include/openssl/des_old.h
createlnk ../../crypto/dh/dh.h ./include/openssl/dh.h
createlnk ../../crypto/dsa/dsa.h ./include/openssl/dsa.h
createlnk ../../crypto/dso/dso.h ./include/openssl/dso.h
createlnk ../../crypto/ebcdic.h ./include/openssl/ebcdic.h
createlnk ../../crypto/ec/ec.h ./include/openssl/ec.h
createlnk ../../crypto/engine/engine.h ./include/openssl/engine.h
createlnk ../../e_os2.h ./include/openssl/e_os2.h
createlnk ../../crypto/err/err.h ./include/openssl/err.h
createlnk ../../crypto/evp/evp.h ./include/openssl/evp.h
createlnk ../../crypto/hmac/hmac.h ./include/openssl/hmac.h
createlnk ../../crypto/idea/idea.h ./include/openssl/idea.h
createlnk ../../crypto/krb5/krb5_asn.h ./include/openssl/krb5_asn.h
createlnk ../../ssl/kssl.h ./include/openssl/kssl.h
createlnk ../../crypto/lhash/lhash.h ./include/openssl/lhash.h
createlnk ../../crypto/md2/md2.h ./include/openssl/md2.h
createlnk ../../crypto/md4/md4.h ./include/openssl/md4.h
createlnk ../../crypto/md5/md5.h ./include/openssl/md5.h
createlnk ../../crypto/mdc2/mdc2.h ./include/openssl/mdc2.h
createlnk ../../crypto/objects/objects.h ./include/openssl/objects.h
createlnk ../../crypto/objects/obj_mac.h ./include/openssl/obj_mac.h
createlnk ../../crypto/ocsp/ocsp.h ./include/openssl/ocsp.h
createlnk ../../crypto/opensslconf.h ./include/openssl/opensslconf.h
createlnk ../../crypto/opensslv.h ./include/openssl/opensslv.h
createlnk ../../crypto/ossl_typ.h ./include/openssl/ossl_typ.h
createlnk ../../crypto/pem/pem2.h ./include/openssl/pem2.h
createlnk ../../crypto/pem/pem.h ./include/openssl/pem.h
createlnk ../../crypto/pkcs12/pkcs12.h ./include/openssl/pkcs12.h
createlnk ../../crypto/pkcs7/pkcs7.h ./include/openssl/pkcs7.h
createlnk ../../crypto/rand/rand.h ./include/openssl/rand.h
createlnk ../../crypto/rc2/rc2.h ./include/openssl/rc2.h
createlnk ../../crypto/rc4/rc4.h ./include/openssl/rc4.h
createlnk ../../crypto/rc5/rc5.h ./include/openssl/rc5.h
createlnk ../../crypto/ripemd/ripemd.h ./include/openssl/ripemd.h
createlnk ../../crypto/rsa/rsa.h ./include/openssl/rsa.h
createlnk ../../crypto/stack/safestack.h ./include/openssl/safestack.h
createlnk ../../crypto/sha/sha.h ./include/openssl/sha.h
createlnk ../../ssl/ssl23.h ./include/openssl/ssl23.h
createlnk ../../ssl/ssl2.h ./include/openssl/ssl2.h
createlnk ../../ssl/ssl3.h ./include/openssl/ssl3.h
createlnk ../../ssl/ssl.h ./include/openssl/ssl.h
createlnk ../../crypto/stack/stack.h ./include/openssl/stack.h
createlnk ../../crypto/symhacks.h ./include/openssl/symhacks.h
createlnk ../../ssl/tls1.h ./include/openssl/tls1.h
createlnk ../../crypto/tmdiff.h ./include/openssl/tmdiff.h
createlnk ../../crypto/txt_db/txt_db.h ./include/openssl/txt_db.h
createlnk ../../crypto/ui/ui_compat.h ./include/openssl/ui_compat.h
createlnk ../../crypto/ui/ui.h ./include/openssl/ui.h
createlnk ../../crypto/x509/x509.h ./include/openssl/x509.h
createlnk ../../crypto/x509v3/x509v3.h ./include/openssl/x509v3.h
createlnk ../../crypto/x509/x509_vfy.h ./include/openssl/x509_vfy.h
