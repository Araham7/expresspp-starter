#!/usr/bin/env bash
# "npm install" for this project (Ubuntu 24.04): system libraries + the C++ packages.
#   1. put the package zips in deps/:  expresspp.zip nevla.zip axiospp.zip nodemailerpp.zip socketiopp.zip bullmqpp.zip cpp-telegram-bot-api.zip nodemonpp.zip
#   2. sudo ./scripts/install-deps.sh
# Everything is installed to /usr/local (find_package finds it there).
set -euo pipefail
cd "$(dirname "$0")/.."
SUDO=""; [ "$(id -u)" -ne 0 ] && SUDO="sudo"
JOBS=$(nproc)

echo "==> system libraries (apt)"
$SUDO apt-get update
$SUDO apt-get install -y --no-install-recommends \
  build-essential cmake pkg-config git curl unzip ca-certificates \
  libdrogon-dev libtrantor-dev libjsoncpp-dev uuid-dev zlib1g-dev libbrotli-dev libc-ares-dev \
  libpq-dev libmariadb-dev libsqlite3-dev libhiredis-dev libyaml-cpp-dev \
  libssl-dev libsodium-dev libcrypt-dev libcurl4-openssl-dev \
  libmongoc-dev libbson-dev

if ! [ -f /usr/local/lib/cmake/mongocxx-3.10.2/mongocxx-config.cmake ] && ! ls /usr/local/lib/cmake/mongocxx* >/dev/null 2>&1; then
  echo "==> mongo-cxx-driver 3.10.2 (needed by nevla)"
  tmp=$(mktemp -d)
  curl -fsSL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.10.2/mongo-cxx-driver-r3.10.2.tar.gz | tar xz -C "$tmp"
  cmake -S "$tmp/mongo-cxx-driver-r3.10.2" -B "$tmp/build" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20 \
        -DBUILD_SHARED_LIBS=ON -DENABLE_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local
  cmake --build "$tmp/build" -j"$JOBS"
  $SUDO cmake --install "$tmp/build"
  rm -rf "$tmp"
fi

echo "==> C++ packages from deps/ (order matters: nevla before expresspp, expresspp before the rest)"
for pkg in nevla expresspp axiospp nodemailerpp socketiopp bullmqpp cpp-telegram-bot-api nodemonpp; do
  zip="deps/$pkg.zip"
  if [ ! -f "$zip" ]; then echo "   skip $pkg (no $zip)"; continue; fi
  tmp=$(mktemp -d)
  unzip -q "$zip" -d "$tmp"
  src=$(dirname "$(find "$tmp" -maxdepth 3 -name CMakeLists.txt | head -1)")
  echo "   building $pkg"
  cmake -S "$src" -B "$src/build" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF >/dev/null
  cmake --build "$src/build" -j"$JOBS"
  $SUDO cmake --install "$src/build" >/dev/null
  rm -rf "$tmp"
done
$SUDO ldconfig
echo "==> done. Next: cp .env.example .env && cmake -S . -B build && cmake --build build -j && ./build/server"
