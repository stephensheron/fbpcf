# FBPCF (Facebook Private Computation Framework)
The Private Computation Framework (PCF) library builds a scalable, secure, and distributed private computation platform to run secure computations on a production level. PCF library supports running the computation on AWS Cloud and is able to integrate various private computation technologies. Specifically, it leverages EMP-toolkit to enable privacy-preserving computations.

## Examples: Private Randomized Controlled Trials
As an interesting example application on top of PCF, we implemented a library that allows developers to perform randomized controlled trials (RCT), without leaking information about who participated or what action an individual took. It uses secure multiparty computation to guarantee this privacy. It is suitable for conducting A/B testing, or measuring advertising lift and learning the aggregate statistics without sharing information on the individual level. See what is RCT [here](docs/PrivateRCT.md) and private lift games pseudocode [here](docs/PrivateLift.md).

## Requirements
### Dependencies for working on Ubuntu 18.04
* Compiling tools
  * gcc 8
  * clang-12
  * cmake 3.12+
* C++ libraries
  * gflags
  * re2
  * folly
* AWS SDK
* EMP toolkit

## Installing Dependencis on Ubuntu 18.04
#### Ubuntu libraries
```
sudo apt-get install -y \
    g++ \
    cmake \
    libboost-all-dev \
    libevent-dev \
    libdouble-conversion-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    libiberty-dev \
    liblz4-dev \
    liblzma-dev \
    libsnappy-dev \
    make \
    zlib1g-dev \
    binutils-dev \
    libssl-dev \
    pkg-config \
    libunwind-dev \
    libjemalloc-dev
```
#### Compiling tools
* gcc 8
```
cd /root || exit
apt-add-repository -y ppa:ubuntu-toolchain-r/test
apt-get update
apt-get install -y gcc-8 g++-8
ln -sf /usr/bin/gcc-8 /usr/bin/gcc
ln -sf /usr/bin/g++-8 /usr/bin/g++
ln -sf /usr/bin/gcc /usr/bin/cc
ln -sf /usr/bin/g++ /usr/bin/c++
```
* clang-12
```
cd /root || exit
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 12
sudo ln -s /usr/bin/clang-12 /usr/bin/clang
sudo ln -s /usr/bin/clang++-12 /usr/bin/clang++
```
* cmake 3.12+
```
cd /root || exit
wget https://cmake.org/files/v3.12/cmake-3.12.3-Linux-x86_64.sh
bash cmake-3.12.3-Linux-x86_64.sh --include-subdir --skip-license
cp cmake-3.12.3-Linux-x86_64/bin/* /usr/bin
cp -r cmake-3.12.3-Linux-x86_64/share/* /usr/share/
rm -r cmake*
```
#### C++ libraries
* gflags
```
cd /root || exit
git clone https://github.com/gflags/gflags
cd gflags || exit
mkdir build && cd build || exit
cmake ..
make && make install
```
* re2
```
cd /root || exit
# TODO: add version control
git clone https://code.googlesource.com/re2
cd re2 || exit
make
make test
sudo make install
sudo make testinstall
```
* Folly and its dependencies
```
# install googletest
cd /root || exit
wget https://github.com/google/googletest/archive/release-1.8.0.tar.gz && \
tar zxf release-1.8.0.tar.gz && \
rm -f release-1.8.0.tar.gz && \
cd googletest-release-1.8.0 || exit && \
cmake . && \
make && \
make install
# install fmt
cd /root || exit
# TODO: add version control
git clone https://github.com/fmtlib/fmt.git
cd fmt || exit
mkdir _build
cd _build || exit
cmake ..
make -j
sudo make install
# install folly
cd /root || exit
git clone https://github.com/facebook/folly.git
cd folly || exit
git checkout v2020.10.12.00
mkdir _build
cd _build || exit
cmake .. -DFOLLY_USE_JEMALLOC=0 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-march=native"
make
sudo make install
```
#### AWS SDK and its dependencies
```
cd /root || exit
sudo apt-get install -y libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev
# TODO: add version control
git clone https://github.com/aws/aws-sdk-cpp.git
cd aws-sdk-cpp || exit
mkdir builds
cd build || exit
# -DCUSTOM_MEMORY_MANAGEMENT=0 is added to avoid Aws::String and std::string issue
# ref: https://github.com/aws/aws-sdk-cpp/issues/416
cmake .. -DBUILD_ONLY="s3;core" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_SHARED_LIBS=OFF -DCUSTOM_MEMORY_MANAGEMENT=0
make
sudo make install
```
#### EMP toolkit
```
#!/bin/bash

# get emp readme scripts
cd /root || exit
git clone https://github.com/emp-toolkit/emp-readme.git
cd emp-readme || exit
git checkout d31ffad00ee86f470dcb12ff50b3d88567577d1f

# install emp dependencies
cd /root || exit
bash ./emp-readme/scripts/install_packages.sh
bash ./emp-readme/scripts/install_relic.sh
#EC STRING SIZE
sed -i "s/FB_POLYN:STRING=283/FB_POLYN:STRING=251/" ~/relic/CMakeCache.txt

# get and install emp-tool
git clone https://github.com/emp-toolkit/emp-tool.git
cd emp-tool || exit
git checkout 9551a53befef2b07e41097bca7fcb3b4aee36262
cd /root/emp-tool || exit
cmake .
make
make install

# get and install emp-ot
cd /root || exit
git clone https://github.com/emp-toolkit/emp-ot.git
cd emp-ot || exit
git checkout a4738c5dc05169400ff68f4eb4e5d51a66263268
cmake .
make
make install

# get and install emp-sh2pc
cd /root || exit
git clone https://github.com/emp-toolkit/emp-sh2pc.git
cd emp-sh2pc || exit
git checkout 07271059d99312cfc0c6589f43fc2d9ddfe6788b
cd /root/emp-sh2pc || exit
mkdir build
cd build || exit
cmake ..
make
make install
```

## Building Aggregator and Lift Game
```
# compile standard lift and aggregator
cd /root || exit
cp /root/CMakeLists.txt /root/emp_games/
cd emp_games || exit
cmake .
make
```

## How to build a docker image that containes game executables using the given docker related files
1. cd to folder `/fbpcf/`
2. build docker image `docker_fbpcf` by running
   - `docker build -t docker_fbpcf -f docker/dockerfile .`
3. now you have built docker image `docker_fbpcf`, you can test run the calculator/aggregator game as publisher or partner
   - open a terminal, play calculator game as the publisher
      - `docker run -ti --network=host docker_fbpcf ./bin/calculator --role=1 --data_directory=/root/lift/calculator/sample_input --input_filename="publisher" --output_path="test_calculator_publisher_output" --use_xor_encryption=false --num_shards=1 --concurrency=1`
   - open another terminal, play calculator game as the partner
      - `docker run -ti --network=host docker_fbpcf ./bin/calculator --role=2 --server_ip=127.0.0.1 --data_directory=/root/lift/calculator/sample_input --input_filename="partner_4_convs" --output_path="test_calculator_partner_output" --use_xor_encryption=false --num_shards=1 --concurrency=1`

## How PCF works
Private Computation Framework enables cryptographic methods that help two parties, Alice and Bob, compute a function on each of their secret inputs and receive outputs without revealing information about the inputs to each other. Specifically, it lets the programmers implement a garbled circuit-based 2pc program.

The garbled circuit protocol happens between a Garbler and an Evaluator, and the function they compute has to be described as a Boolean circuit. Garbler compiles the function into a Boolean circuit and adds their inputs to the circuit. Garbler then garbles the circuit, which hides the value of the inputs using encryption. Garbler sends the garbled circuit to Evaluator, who adds their inputs and computes the circuit. After the computation, Garbler and Evaluator will work together to get back the output of computation in clear text.

We looked into multiple services that enable secure computation to decide on the best framework that meets the requirements of Private RCT. After comparing more than twenty secure computation services — across secret sharing, garbled circuits, and homomorphic encryption-based protocols, we chose EMP-toolkit as the best fit for our application.

## Design Principal
1. Simplicity. We hide details of C++ and MPC as much as possible. We hope every engineer is able to write an MPC game with minimal ramp-up effort.
2. Efficiency. We provide many common utilities for engineers to write a game with ease. We embrace functional programming to speed up development.
3. Test and quality. We believe testing is important given the complexity of MPC. We provide a test framework for engineers to write unit tests to validate their games. We developed a special QueueIO for two parties to communicate via an in-memory queue. This allows MPC games to be implemented as pure functions, which also makes testing easier than before.

## Architecture

<img src="docs/pcfArch.jpg" alt="Figure 1: Architecture of an appplication on top of
PCF" width="30%" height="30%">


### main
main is the main function in C++. It is responsible for dealing with command line arguments.

### EMPApp
We defined a unified interface. As you can read the code below, it’s responsible for dealing with input data and output data. It’s also responsible for launching a game.


```
template <class GameType, class InputDataType, class OutputDataType>

class EmpApp {
 public:
   virtual void run() {
 };

 protected:
   virtual InputDataType getInputData() = 0;
   virtual void putOutputData(const OutputDataType& output) = 0;

 private:
   Role role_;
   std::string serverIp_;
   uint16_t port_;
};
```

### EMPGame

EMPGame is another unified interface. It defines MPC role and exposes a `play` method which is as simple as taking some input data and return the output data after MPC computation.

```
enum class Role { Alice = emp::ALICE, Bob = emp::BOB };

template <class IOChannel, class InputDataType, class OutputDataType>
class EmpGame : public IMpcGame<InputDataType, OutputDataType> {
 protected:
   Role role_;

 private:
   std::unique_ptr<IOChannel> ioChannel_;
 };

class IMpcGame {
 public:
   virtual const OutputDataType play(const InputDataType& inputData) const = 0;
};
```
### Other components in PCF
**/mpc**
- EmpVector
- EmpTestUtil
- QueueIO

**/common**
- Functional map/reduce
- Vector operators

**/io**
- LocalFileManager
- S3FileManager
- FileManagerUtils
- MockedFileManger for testing

**/aws**
- AwsSdk singleton
- S3Util
- Mocked S3 client


## Full documentation
See full documentation [here](docs)

## Join the PCF community
* Website: https://github.com/facebookresearch/fbpcf

See the [CONTRIBUTING](CONTRIBUTING.md) file for how to help out.

## License
PCF and private RCT are [MIT](LICENSE) licensed, as found in the LICENSE file.