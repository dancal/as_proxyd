as_proxyd README
==============

Compilation and Installation
----------------------------

* aerospike-client-c
-----------------------------::

$ cd libs/aerospike-client-c
$ git submodule init
$ git submodule update
$ make

* boost install
-----------------------------::

$ cd libs/boost_1_58_0
$ ./bootstrap.sh
$ ./b2

* jansson
-----------------------------::

$ cd libs/jansson
$ ./release.sh
$ ./configure
$ make

* jemalloc
-----------------------------::

$ cd libs/jemalloc
$ ./autogen.sh
$ ./configure

* as_proxyd
-----------------------------::

$ make
$ vi conf/config.ini
$ ./bin/as_proxyd


== GET EXAMPLE

http://172.16.6.32:4000/GET?ns=viewer&set=COOKIE&key=test

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
    curl_setopt($ch, CURLOPT_URL, 'http://172.16.6.32:4000/GET?ns=viewer&set=COOKIE&key=test');
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
    curl_setopt($ch, CURLOPT_URL, 'http://172.16.6.32:4000/PUT?ns=viewer&set=COOKIE&key=test');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $pData);
    $response = curl_exec($ch);
 ?>

### REMOVE TEST [ PHP ]

code::

 <?php
    $ch     = curl_init();
    curl_setopt($ch, CURLOPT_URL, 'http://172.16.6.32:4000/DEL?ns=viewer&set=COOKIE&key=test');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    $response = curl_exec($ch);
 ?>
