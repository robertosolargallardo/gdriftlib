Description
-----------
The libgdrift simulation  kernel  is  a  library  developed to  simulate  forward-in-time  population  genetics  based  on  the Wright-Fisher model. libgdrift provides  a  set  of  basico perations  (events)  that  may  be  combined  in  order  to  modeland   simulate   complex   demographics   scenarios. libgdrift aims to perform as-fast-as-possible forward-in-time simulation by optimizing memory usage and temporal/spatial locality.

Build
-----
```
git clone https://github.com/robertosolargallardo/libgdrift.git
mkdir libgdrift/build
cd libgdrift/build
cmake ..
sudo make install
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.:/usr/local/lib
```
