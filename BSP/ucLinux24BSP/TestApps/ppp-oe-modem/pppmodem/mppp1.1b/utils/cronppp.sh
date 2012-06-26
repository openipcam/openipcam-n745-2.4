#!/bin/sh
##############
# start ppp from cron
# muquit, Jun-26-1997
#############
ppp_command="/users/muquit/mysrc/mppp1.1b/mppp"
cronppp_log="/dev/null"
mppp_config="/users/muquit/.mpppdir/beta.cfg"

$ppp_command -quit 1 -config $mppp_config >> $cronppp_log 2>&1

if [ $? -eq 0 ]; then
    echo "ppp connection apparently succeeded" >>  $cronppp_log 2>&1
else
    echo "ppp connection apparently failed" >>  $cronppp_log 2>&1
fi

