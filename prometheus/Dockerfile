# Dockerfile for Prometheus
# docker build -t custom-prometheus .

FROM prom/prometheus:latest

COPY prometheus.yaml /etc/prometheus/prometheus.yaml

EXPOSE 9090

ENTRYPOINT [ "/bin/prometheus" ]
CMD [ "--config.file=/etc/prometheus/prometheus.yaml", \
      "--storage.tsdb.path=/prometheus", \
      "--web.console.libraries=/usr/share/prometheus/console_libraries", \
      "--web.console.templates=/usr/share/prometheus/consoles" ]
