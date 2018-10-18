export default class logger {

    static get LOGTYPE_INFO() {
        return 0;
    }
    static get LOGTYPE_WARN() {
        return 1;
    }
    static get LOGTYPE_ERRO() {
        return 2;
    }

    static init(console) {
        logger.console = console;
        logger.logcache = new Array();
    };

    static info(logstr) {
        logger.logcache.unshift([logger.LOGTYPE_INFO, logstr]);
        console.info(logstr);
        if(logger.console)
        {
            logger.refreshLog();
        }
    };

    static warn(logstr) {
        logger.logcache.unshift([logger.LOGTYPE_WARN, logstr]);
        console.warn(logstr);
        if(logger.console)
        {
            logger.refreshLog();
        }
    };

    static error(logstr) {
        logger.logcache.unshift([logger.LOGTYPE_ERRO, logstr]);
        console.error(logstr);
        if(logger.console)
        {
            logger.refreshLog();
        }
    };

    static refreshLog() {
        while (logger.console.firstChild) {
            logger.console.removeChild(logger.console.firstChild);
        }

        if (logger.logcache.length > 50) {
            var deleteCount = logger.logcache.length - 50;
            logger.logcache.splice(50, deleteCount);
        }

        var count = 0;
        logger.logcache.forEach(function (line) {
            var log_line = document.createElement('div');
            if (count++ % 2 === 0) {
                log_line.className = 'log-line-s';
            }
            else {
                log_line.className = 'log-line-d';
            }

            var infotype = line[0];
            var log_icon = document.createElement('i');

            switch (infotype) {
                case logger.LOGTYPE_INFO:
                    log_icon.className = 'btm bt-info-circle';
                    log_line.style.color = '#fff';
                    break;
                case logger.LOGTYPE_WARN:
                    log_icon.className = 'bts bt-exclamation-triangle';
                    log_line.style.color = '#ff3';
                    break;
                case logger.LOGTYPE_ERRO:
                    log_icon.className = 'bts bt-ban';
                    log_line.style.color = '#f33';
                    break;
            }

            log_line.appendChild(log_icon);

            var log_text = document.createTextNode(' ' + line[1]);
            log_line.appendChild(log_text);

            logger.console.appendChild(log_line);
        });
    };
}