.PHONY: help develop build test
.DEFAULT_GOAL := help
NGINX_VERSION=1.20.0
NGINX_TARBALL="nginx-$(NGINX_VERSION).tar.gz"
BUILD_DIR="$(PWD)/build/nginx"
VENDOR_DIR="$(PWD)/vendor/nginx-$(NGINX_VERSION)"

help:
	@sed -ne 's/\(^[^#]*\):\s*##\(.*\)/\1\t\t\2/p' $(MAKEFILE_LIST)

develop:  ## prepare development environment
	curl -s -L "https://raw.githubusercontent.com/kward/shunit2/master/shunit2" > tests/shunit2
	mkdir -p $(BUILD_DIR) > /dev/null 2>&1
	mkdir -p $(PWD)/vendor > /dev/null 2>&1
	curl -s -L "http://nginx.org/download/$(NGINX_TARBALL)" > "./vendor/$(NGINX_TARBALL)"
	tar -C "./vendor" -xzf "./vendor/$(NGINX_TARBALL)"

clean:  ## clean up environment
	-rm -rf build vendor tests/shunit2

build:  ## build nginx with module
	cd $(VENDOR_DIR) && CFLAGS="-g -O0" ./configure \
	    --with-debug \
	    --prefix="$(BUILD_DIR)" \
	    --conf-path=conf/nginx.conf \
	    --error-log-path=logs/error.log \
	    --http-log-path=logs/access.log \
	    --add-module=../../
	cd "$(VENDOR_DIR)" && make build install
	ln -sf "$(PWD)/nginx.conf" "$(BUILD_DIR)/conf/nginx.conf"

test:  ## run tests
	sh ./tests/test_ngx_module.sh
