# /infra/docker/Dockerfile

# Base Python image
FROM python:3.11-slim

# Set environment variables
ENV PYTHONDONTWRITEBYTECODE 1
ENV PYTHONUNBUFFERED 1
ENV PYTHONPATH="/app/src"

# Set working directory
WORKDIR /app

# Install dependencies
COPY requirements.txt /app/
RUN pip install --upgrade pip \
    && pip install -r requirements.txt

# Copy project source code
COPY src/ /app/src/
COPY config/ /app/config/
COPY infra/monitoring/ /app/monitoring/

# Expose metrics port
EXPOSE 9100

# Set default command to run Live Controller
CMD ["python", "-m", "main.live_controller"]
