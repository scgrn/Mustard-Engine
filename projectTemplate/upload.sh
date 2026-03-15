#!/bin/bash

source project.cfg

rsync -avz --progress bin/web-release/ "$USER@$SERVER:$REMOTE_PATH"
