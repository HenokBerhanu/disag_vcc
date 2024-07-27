# Dockerfile for Grafana
# docker build -t custom-grafana .

FROM grafana/grafana:latest

EXPOSE 3000

ENV GF_SECURITY_ADMIN_USER=admin
ENV GF_SECURITY_ADMIN_PASSWORD=admin

ENTRYPOINT [ "/run.sh" ]
