#include "config.h"
#include "BooleanSolutor.h"

#include <string>
#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

int main(int argc, char *argv[]) {
    log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4c.properties"));

    if(argc != 2) {
        LOG4CPLUS_ERROR(__FUNCTION__, LOG4CPLUS_TEXT("expected logical expression, e.g: (42 > -42) && !false && (true || false)"));
        return 1;
    }

    BooleanSolutor boolean_solutor;

    try {
        auto res = boolean_solutor.solve(argv[1]);
        LOG4CPLUS_INFO(__FUNCTION__, LOG4CPLUS_TEXT(argv[1]) << "=" << LOG4CPLUS_TEXT((res ? "true" : "false")));
    } catch (std::exception &e ) {
        LOG4CPLUS_ERROR(__FUNCTION__, LOG4CPLUS_TEXT(e.what()));
    }

    return 0;
}