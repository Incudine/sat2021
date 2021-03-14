# sat2021

This repository includes code I submitted to the SAT 2021 conference as part of my publication "Hash-based preprocessing and inprocessing techniques in SAT solvers". I provide implementations of three processing techniques: Subsumption, Blocked Clause Elimination and Bounded Variable Elimination as detailed in the paper. The methods were tested on the 2020 SAT competition benchmarks, which you can download from https://satcompetition.github.io/2020/downloads.html (it's ~30GB unzipped, so I did not include it here).

If you would like to reproduce the experiments of the paper, download the Main Track benchmark dataset from https://satcompetition.github.io/2020/downloads.html and place them in the folder /path_to_repository/sat2021/build/test.

Build instructions:\
cd build\
cmake ..\
cmake --build --config Release

Run using:\
./sat
