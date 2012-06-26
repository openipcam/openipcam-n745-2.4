all:
	cd ./openssl; make -f makefile-arm; cd ..;
	cd ./boa-dev; make -f makefile-arm; cd ..;

clean:
	cd ./openssl; make -f makefile-arm clean; cd ..;
	cd ./boa-dev; make -f makefile-arm clean; cd ..;
	find ./Library -regex '.*\.[ha]' | xargs rm -f 
