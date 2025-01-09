#!/bin/bash

# add upstream repository
if [ -z "$(git remote -v | grep upstream | head -n 1)" ]; then
	echo -e "\nadd upstream repository"
	git remote add upstream https://github.com/u-boot/u-boot.git
fi

# add user name and e-mail
if [ -z "$(git config --list --local | grep user.name)" ]; then
	echo -e "\nadd user name and e-mail"
	git config --local user.name "Shiji Yang"
	git config --local user.email "yangshiji66@outlook.com"
fi

echo -e "\nfetch origin"
git fetch origin

echo -e "\nfetch upstream"
git fetch upstream

echo -e "\nsync master branch:"
git push origin upstream/master:master

echo -e "\nsync next branch:"
git push origin upstream/next:next

echo -e "\nsync tags:"
git push --tags origin
