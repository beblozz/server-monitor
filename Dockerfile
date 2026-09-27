FROM ubuntu:24.04 AS builder

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    libboost-system-dev \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -S . -B build
RUN cmake --build build


FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libboost-system1.83.0 \
    libpq5 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/server-monitor /app/server-monitor
COPY --from=builder /app/config /app/config
COPY --from=builder /app/web /app/web

EXPOSE 8080

CMD ["/app/server-monitor"]