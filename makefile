###############################################################################
# Copyright 1998-2021 NetBurner, Inc.  ALL RIGHTS RESERVED
#
#    Permission is hereby granted to purchasers of NetBurner Hardware to use or
#    modify this computer program for any use as long as the resultant program
#    is only executed on NetBurner provided hardware.
#
#    No other rights to use this program or its derivatives in part or in
#    whole are granted.
#
#    It may be possible to license this or other NetBurner software for use on
#    non-NetBurner Hardware. Contact sales@Netburner.com for more information.
#
#    NetBurner makes no representation or warranties with respect to the
#    performance of this computer program, and specifically disclaims any
#    responsibility for any damages, special or consequential, connected with
#    the use of this program.
#
# NetBurner
# 16855 W Bernardo Dr #260
# San Diego, CA 92127
# www.netburner.com
###############################################################################

#This is a minimal make file.
#anything that starts with a # is a comment
#
#it should be easy to maintain.
#to generate the dependancies automatically 
#run "make depend"
#
#
#To clean up the directory 
#run "make clean"
#your responsibilities as a programmer:
#
# Run make depend whenever:
#	You add files to the project
#	You change what files are included in a source file
#
# make clean whenever you change this makefile.
#

#Setup the project root name
#This will built NAME.x and save it as $(NBROOT)/bin/NAME.x
NAME	= tcp2serial
#CSRCS   := main.c
#Uncomment and modify these lines if you have CPP or S files.
CXXSRCS := main.cpp web.ccp
#ASRCS := foo.s
CXXSRCS += htmldata.cpp
CREATEDTARGS = htmldata.cpp

XTRALIB += $(NBROOT)/lib/cryptolib.a
DBXTRALIB += $(NBROOT)/lib/DBcryptolib.a

#include the file that does all of the automagic work!
include $(NBROOT)/make/main.mak

htmldata.cpp : $(wildcard html/*.*)
	comphtml html -ohtmldata.cpp

