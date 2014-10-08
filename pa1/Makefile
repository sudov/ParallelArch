all: 
	cd sequential && make && ./run && cd ../
	cd parallel && make && ./run && cd ../

sequential: 
	cd sequential && make && ./run && cd ../

parallel: 
	cd parallel && make && ./run && cd ../

clean:
	cd sequential && make clobber && cd ../
	cd parallel && make clobber && cd ../

