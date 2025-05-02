# ----------------------
# Final Runtime Stage Only (Fast Build Using Prebuilt Wheels)
# ----------------------
    FROM python:3.11-slim

    # Environment variables
    ENV PYTHONDONTWRITEBYTECODE=1 \
        PYTHONUNBUFFERED=1 \
        PYTHONPATH="/app/src"
    
    WORKDIR /app
    
    # System dependencies for PostgreSQL
    RUN apt-get update && apt-get install -y \
        libpq-dev \
     && apt-get clean && rm -rf /var/lib/apt/lists/*
    
    # Install from prebuilt wheel cache
    COPY requirements.txt .
    COPY wheels-linux/ /wheels/
    RUN pip install --no-cache-dir --default-timeout=180 --find-links=/wheels -r requirements.txt
    
    # Copy source code
    COPY src/ /app/src/
    COPY config/ /app/config/
    COPY infra/monitoring/ /app/monitoring/
    
    # Expose Prometheus metrics port
    EXPOSE 9100
    
    # Run live controller
    CMD ["python", "-m", "main.live_controller"]
    