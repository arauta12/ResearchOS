if [ ! -e "temp.vdi" ]; then
    VBoxManage convertfromraw ros.iso temp.vdi
fi
