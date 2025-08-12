#########################################################
# Build First Stage
#########################################################
FROM ubuntu:latest AS cmake_build

LABEL Name=cmake_build Version=0.0.1

# Set environment variables for non-interactive apt-get installs
ENV DEBIAN_FRONTEND=noninteractive

# Update package lists and install required dependencies
# Add python3-venv for creating isolated Python environments.
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    git \
    build-essential \
    libssl-dev \
    lcov \
    python3 \
    python3-venv && \
    rm -rf /var/lib/apt/lists/*

# Clone CMake repository and switch to the desired version (3.26 in this case)
RUN git clone https://gitlab.kitware.com/cmake/cmake.git /tmp/cmake_src && \
    cd /tmp/cmake_src && \
    git checkout v3.26.3 && \
    ./bootstrap && \
    make -j$(nproc) && \
    make install && \
    cd / && \
    rm -rf /tmp/cmake_src

# Create a Python virtual environment and install gcovr into it.
# This prevents the "externally-managed-environment" error.
ENV VIRTUAL_ENV="/opt/venv"
RUN python3 -m venv $VIRTUAL_ENV && \
    $VIRTUAL_ENV/bin/pip install --no-cache-dir gcovr

# Add the virtual environment's bin directory to the PATH
# This makes 'gcovr' available globally within this Docker stage and subsequent stages
# by simply calling 'gcovr' instead of '$VIRTUAL_ENV/bin/gcovr'.
ENV PATH="$VIRTUAL_ENV/bin:/usr/local/bin:${PATH}"

#########################################################
# Build Second Stage
#########################################################
FROM cmake_build AS cmake_app

LABEL Name=cmake_app Version=0.0.1

ARG use_docker=false
ENV USE_DOCKER=$use_docker

WORKDIR /app

# Copy the entrypoint script
COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# Set the entrypoint
ENTRYPOINT ["/entrypoint.sh"]
