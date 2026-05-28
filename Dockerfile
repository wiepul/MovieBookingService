# syntax=docker/dockerfile:1

FROM ubuntu:24.04 AS base

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    ninja-build \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

FROM base AS build
RUN cmake -S . -B build -G Ninja -DBUILD_TESTING=ON
RUN cmake --build build -j

FROM build AS test
RUN ctest --test-dir build --output-on-failure

FROM ubuntu:24.04 AS runtime
WORKDIR /app
COPY --from=build /app/build/bin/movie_booking_cli /usr/local/bin/movie_booking_cli

ENTRYPOINT ["/usr/local/bin/movie_booking_cli"]
