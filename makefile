src: FORCE
	cd ./src/ && make

tests: FORCE
	cd ./tests/ && make run

clean: FORCE
	cd ./src/ && make clean
	cd ./tests/ && make clean

FORCE: