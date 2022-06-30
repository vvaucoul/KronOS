#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <commit>"
  exit 1
fi

make fclean
git add --all ; git commit -am "$1" ; git push
