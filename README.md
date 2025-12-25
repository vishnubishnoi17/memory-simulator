# Memory Management Simulator

## Build
make

## Run
./memsim
> init memory 1024
> set allocator first_fit
> malloc 100
> malloc 200
> free 1
> dump memory
> stats
> exit

## Tests
make test
./tests

## Optional Features
- Buddy: `set allocator buddy` (init with power-of-2 size)
- Cache/VM: Edit init() flags to true, use `access 0x1234`
