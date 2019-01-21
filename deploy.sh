#!/bin/bash

docker build -f docker/base.Dockerfile -t amazon-echo-server:base .
docker build -f docker/Dockerfile -t my_image .
gcloud builds submit --config docker/cloudbuild.yaml .
