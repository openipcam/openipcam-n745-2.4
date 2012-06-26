#!/bin/bash

if [[ $1 == '--help' ]]; then
	echo "Create certificate and private key for boa."
	exit
fi

openssl genrsa -out ssl_key.pem 1024
openssl req -new -out ssl_req.csr -key ssl_key.pem
openssl x509 -req -in ssl_req.csr -out ssl_cert.pem -signkey ssl_key.pem -days 365
