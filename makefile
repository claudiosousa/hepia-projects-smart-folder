src:
	cd ./src/ && make

tests:
	cd ./tests/ && make run

clean:
	cd ./src/ && make clean
	cd ./tests/ && make clean
