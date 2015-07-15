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

* centos
-----------------------------::

$ sudo yum install openssl-devel glibc-devel autoconf automake libtool
$ sudo yum install lua-devel 

* boost install [ http://www.boost.org/ ]
-----------------------------::

$ cd libs
$ wget "http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz?r=http%3A%2F%2Fwww.boost.org%2Fusers%2Fhistory%2Fversion_1_58_0.html&ts=1436937714&use_mirror=jaist" -O boost_1_58_0.tar.gz
$ tar xvzf boost_1_58_0.tar.gz
$ cd boost_1_58_0
$ ./bootstrap.sh
$ ./b2


* aerospike-client-c [ https://github.com/aerospike/aerospike-client-c ]
-----------------------------::

$ cd ..
$ git clone https://github.com/aerospike/aerospike-client-c.git
$ cd aerospike-client-c
$ git submodule init
$ git submodule update
$ make

* jansson [ http://jansson.readthedocs.org/en/latest/index.html ]
-----------------------------::

$ cd ..
$ git clone https://github.com/akheron/jansson.git
$ cd jansson
$ ./release.sh
$ ./configure
$ make

* jemalloc [ http://www.canonware.com/jemalloc/ ]::

$ cd ..
$ git clone https://github.com/jemalloc/jemalloc.git
$ cd jemalloc
$ ./autogen.sh
$ ./configure

* as_proxyd
-----------------------------::

$ make
$ vi conf/config.ini
$ ./bin/as_proxyd


## Example

== GET EXAMPLE

http://172.16.6.32:4000/GET?ns=test_ns&set=test_set&key=test

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

== GET TEST [ PHP ]

code::

 <?php
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, 'http://172.16.6.32:4000/GET?ns=test_ns&set=test_set&key=test');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    $response = curl_exec($ch);
    var_dump($response);
 ?>

### PUT TEST [ PHP ]

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

### REMOVE TEST [ PHP ]

code::

 <?php
    $ch     = curl_init();
    curl_setopt($ch, CURLOPT_URL, 'http://172.16.6.32:4000/DEL?ns=test_ns&set=test_set&key=test');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    $response = curl_exec($ch);
 ?>
