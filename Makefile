
.FORCE:

build: .FORCE
	cmake -S . -B $@

build-shared: .FORCE
	cmake -S . -B $@ -DBUILD_SHARED_LIBS=YES -DCMAKE_BUILD_TYPE=Release
	cmake --build $@

build-static: .FORCE
	cmake -S . -B $@ -DBUILD_SHARED_LIBS=NO -DCMAKE_BUILD_TYPE=Release
	cmake --build $@

# use `make install TESTING="--prefix _install"` to dry run installation
TESTING ?=

install: build-static build-shared
	cmake --install build-shared $(TESTING)
	cmake --install build-static $(TESTING)

clean: .FORCE
	git clean -ifdx