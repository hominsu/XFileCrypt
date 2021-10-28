#build stage
FROM alpine:latest as builder
# apk 换源
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.aliyun.com/g' /etc/apk/repositories && apk update && apk upgrade
# 安装依赖：gcc、g++、make、openssl、c-ares、cmake、grpc、protobuf、protoc、nlohmann-json
RUN apk --no-cache add build-base openssl-dev openssl-libs-static cmake c-ares-dev
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp/
WORKDIR /usr/src/myapp/build
RUN ./gen.sh

#final stage
FROM alpine:latest
# apk 换源
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.aliyun.com/g' /etc/apk/repositories && apk update && apk upgrade
# 安装依赖
RUN apk --no-cache add tzdata c-ares
COPY --from=builder /usr/src/myapp/build/XFileCrypt /app/bin/XFileCrypt
WORKDIR /app/bin/
ENTRYPOINT ./XFileCrypt -e /src /enc 123456
LABEL Name=hominsu/XFileCrypt Version=1.0
