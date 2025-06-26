FROM ubuntu:24.04

WORKDIR /app
COPY . .
RUN apt-get update && apt-get install -y build-essential \
    && apt-get clean && rm -rf /var/lib/apt/lists/*
RUN make

# Install Python + venv
RUN apt-get update && apt-get install -y python3 python3-pip python3-venv \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Create venv and install dependencies in it
RUN python3 -m venv /app/venv
ENV PATH="/app/venv/bin:$PATH"
COPY requirements.txt .
RUN pip install --upgrade pip
RUN pip install -r requirements.txt

# Copy the rest of your files and set entrypoint
COPY serve.py .

# Expose the port your app runs on
EXPOSE 5000

ENTRYPOINT ["python", "serve.py"]