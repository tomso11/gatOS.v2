FROM debian:jessie 

RUN apt-get update && apt-get -y install locales nasm llvm clang gcc qemu make ssh

WORKDIR /root
VOLUME ["/root/ArquiSO"]
CMD ["bash"]
