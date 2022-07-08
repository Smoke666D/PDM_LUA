# Static Checkers options
REPORT_DIR     = reports
CHKREPORT	     = $(REPORT_DIR)/cppcheck-report.txt
CHKFLAGS	     = --enable=all --error-exitcode=1 --std=c11 --suppressions-list=cppcheck-supress.txt -D__GNUC__
CHKMISRAREPORT = $(REPORT_DIR)/misra-report.txt
CHKMISRAFLAGS  = -q -j4 --addon=misra --std=c11 --suppress=misra-c2012-2.7