as_proxyd
==============

as_proxyd is a http proxy for aerospike.

- Simple and intuitive API and data model
- Proxy which translates http protocol to Aerospike protocol
- A connection pool for Aeropsike [ http://www.aerospike.com/ ].

as_proxyd is licensed under the MIT license; see LICENSE in the source distribution for details.


Description
----------------------------


Compilation and Installation
----------------------------

centos install::

$ sudo yum install openssl-devel glibc-devel autoconf automake libtool
$ sudo yum install lua-devel 

boost install [ http://www.boost.org/ ] install::

$ cd libs
$ wget "http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz?r=http%3A%2F%2Fwww.boost.org%2Fusers%2Fhistory%2Fversion_1_58_0.html&ts=1436937714&use_mirror=jaist" -O boost_1_58_0.tar.gz
$ tar xvzf boost_1_58_0.tar.gz
$ cd boost_1_58_0
$ ./bootstrap.sh
$ ./b2


aerospike-client-c [ https://github.com/aerospike/aerospike-client-c ] install::

$ cd ..
$ git clone https://github.com/aerospike/aerospike-client-c.git
$ cd aerospike-client-c
$ git submodule init
$ git submodule update
$ make

jansson [ http://jansson.readthedocs.org/en/latest/index.html ] install::

$ cd ..
$ git clone https://github.com/akheron/jansson.git
$ cd jansson
$ ./release.sh
$ ./configure
$ make

jemalloc [ http://www.canonware.com/jemalloc/ ] install::

$ cd ..
$ git clone https://github.com/jemalloc/jemalloc.git
$ cd jemalloc
$ ./autogen.sh
$ ./configure

as_proxyd install::

$ make
$ vi conf/config.ini
$ ./bin/as_proxyd


as_proxyd config
-------------

conf/config.ini::

 [server]
 host        = 0.0.0.0
 port        = 8000
 max_thread  = 8
 status      = /path/alive.html

 [aerospike]
 servers     = "172.16.xx.xx,172.16.xx.xx";
 port        = 3000
 timeout     = 20

GET EXAMPLE
-------------

http://172.16.xx.xx:8000/GET?ns=test_ns&set=test_set&key=test

output::

 {
    as_bins: {
		gender: 1,
		username: "user1",
		password: "passuser1"
	},
	as_status_msg: "AEROSPIKE_OK",
	as_ttl: 604785,
	as_gen: 8,
	as_status_code: 0,
	proxy_latency: 0.000115,
	proxy_status: "OK",
	proxy_code: 200
 }

GET TEST [ PHP ]
-------------

code::

 <?php
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, 'http://172.16.xx.xx:8000/GET?ns=test_ns&set=test_set&key=test');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    $response = curl_exec($ch);
    var_dump($response);
 ?>

PUT TEST [ PHP ]
-------------

code::

 <?php
    $post = [
        'username' => 'user1',
        'password' => 'passuser1',
        'gender'   => 1,
    ];

    $pData  = json_encode($post);
    $ch     = curl_init();
    curl_setopt($ch, CURLOPT_URL, 'http://172.16.6.32:4000/PUT?ns=test_ns&set=test_set&key=test');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $pData);
    $response = curl_exec($ch);
 ?>

REMOVE TEST [ PHP ]
-------------

code::

 <?php
    $ch     = curl_init();
    curl_setopt($ch, CURLOPT_URL, 'http://172.16.6.32:4000/DEL?ns=test_ns&set=test_set&key=test');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    $response = curl_exec($ch);
 ?>


AB TEST
-------------

result::

 $ ab -k -n 10000000 -c 1024 -t 300 "http://172.16.xx.xx:8000/GET?ns=test_ns&set=test_set&key=test"

 This is ApacheBench, Version 2.0.40-dev <$Revision: 1.146 $> apache-2.0
 Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
 Copyright 2006 The Apache Software Foundation, http://www.apache.org/

 Benchmarking 172.16.6.32 (be patient)
 Completed 5000 requests
 Completed 10000 requests
 Completed 15000 requests
 Completed 20000 requests
 Completed 25000 requests
 Completed 30000 requests
 Completed 35000 requests
 Completed 40000 requests
 Completed 45000 requests
 Finished 50000 requests

 Server Software:        
 Server Hostname:        172.16.6.32
 Server Port:            8000

 Document Path:          /GET?ns=test_ns&set=test_set&key=test
 Document Length:        200 bytes

 Concurrency Level:      1024
 Time taken for tests:   0.740346 seconds
 Complete requests:      50000
 Failed requests:        0
 Write errors:           0
 Keep-Alive requests:    50000
 Total transferred:      15500930 bytes
 HTML transferred:       10000600 bytes
 Requests per second:    67535.99 [#/sec] (mean)
 Time per request:       15.162 [ms] (mean)
 Time per request:       0.015 [ms] (mean, across all concurrent requests)
 Transfer rate:          20445.85 [Kbytes/sec] received

 Connection Times (ms)
               min  mean[+/-sd] median   max
 Connect:        0    0   2.1      0      25
 Processing:     3   13   7.8     13     290
 Waiting:        3   13   7.8     13     289
 Total:          3   13   8.3     13     309

 Percentage of the requests served within a certain time (ms)
  50%     13
  66%     14
  75%     15
  80%     16
  90%     20
  95%     25
  98%     29
  99%     38
 100%    309 (longest request)
