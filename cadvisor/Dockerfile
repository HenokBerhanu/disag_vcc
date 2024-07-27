# Dockerfile for CAdvisor
# docker build -t custom-cadvisor .

FROM gcr.io/cadvisor/cadvisor:latest

EXPOSE 8080

ENTRYPOINT [ "/usr/bin/cadvisor" ]
CMD [ "--logtostderr" ]
