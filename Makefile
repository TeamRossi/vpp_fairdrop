SHELL := /bin/bash

setup:
	./scripts/setup.sh
	./scripts/dpdk_readme.sh
clean:
	./scripts/clean.sh

run-cpu-experiments-otcs:
	./scripts/experiment-cpu.sh otcs

run-cpu-experiments-networking:
	./scripts/experiment-cpu.sh networking

run-bandwidth-experiments-otcs:
	./scripts/experiment-bw.sh otcs

run-bandwidth-experiments-networking:
	./scripts/experiment-bw.sh networking
