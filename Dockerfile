# Multi-stage build: compile in a full image, run in a small one.
#   docker build -t my-server .        (needs the package zips in deps/, see scripts/install-deps.sh)
FROM ubuntu:24.04 AS build
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /src
COPY scripts/install-deps.sh scripts/
COPY deps/ deps/
RUN ./scripts/install-deps.sh
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF && cmake --build build -j"$(nproc)"

FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive NODE_ENV=production
RUN apt-get update && apt-get install -y --no-install-recommends \
      libdrogon1t64 libjsoncpp25 libsodium23 libcurl4t64 libhiredis1.1.0 libmongoc-1.0-0t64 ca-certificates \
    && rm -rf /var/lib/apt/lists/*
COPY --from=build /usr/local/lib/ /usr/local/lib/
RUN ldconfig
WORKDIR /app
COPY --from=build /src/build/server ./server
COPY public/ public/
COPY views/ views/
RUN mkdir -p uploads logs && useradd -r -u 10001 app && chown -R app /app
USER app
EXPOSE 3000
CMD ["./server"]
