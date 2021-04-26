#! /bin/sh

test_responds_with_hello_world() {
    result=$(curl --silent http://127.0.0.1:8080/)

    assertTrue $?
    assertEquals "$result" "Hello World"
}

oneTimeSetUp() {
    set +e
    ./build/nginx/sbin/nginx
}

oneTimeTearDown() {
    ./build/nginx/sbin/nginx -s stop 2> /dev/null || true
}
# Load shUnit2
. "$(dirname $(realpath $0))/shunit2"
