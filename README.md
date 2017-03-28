The libgdrift simulation  kernel  is  a  library  developed to  simulate  forward-in-time  population  genetics  based  on  the Wright-Fisher model. libgdrift provides  a  set  of  basico perations  (events)  that  may  be  combined  in  order  to  modeland   simulate   complex   demographics   scenarios. libgdrift aims to perform as-fast-as-possible forward-in-time simulation by optimizing memory usage and temporal/spatial locality.

Build
-----

```bash
git clone --recursive https://github.com/corvusoft/restbed.git
mkdir restbed/build
cd restbed/build
cmake [-DBUILD_TESTS=YES] [-DBUILD_EXAMPLES=YES] [-DBUILD_SSL=NO] [-DBUILD_SHARED=YES] [-DCMAKE_INSTALL_PREFIX=/output-directory] ..
make [-j CPU_CORES+1] install
make test
