all:
	cd ./src/ && make

tests: all
	cd ./tests/ && make
	cd ./tests/ && make run

clean:
	cd ./src/ && make clean
	cd ./tests/ && make clean
