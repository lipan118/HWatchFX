package com.vise.log.common;

import android.annotation.SuppressLint;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * @Description: 正则表达式验证格式化输出
 * @author: <a href="http://www.xiaoyaoyou1212.com">DAWI</a>
 * @date: 16/12/11 13:41.
 */
public abstract class LogPattern {

    private final int count;
    private final int length;

    private LogPattern(int count, int length) {
        this.count = count;
        this.length = length;
    }

    public final String apply(StackTraceElement caller) {
        String string = doApply(caller);
        return LogConvert.shorten(string, count, length);
    }

    protected abstract String doApply(StackTraceElement caller);

    protected boolean isCallerNeeded() {
        return false;
    }

    public static LogPattern compile(String pattern) {
        try {
            return pattern == null ? null : new Compiler().compile(pattern);
        } catch (Exception e) {
            return new PlainLogPattern(0, 0, pattern);
        }
    }

    public static class PlainLogPattern extends LogPattern {

        private final String string;

        public PlainLogPattern(int count, int length, String string) {
            super(count, length);
            this.string = string;
        }

        @Override
        protected String doApply(StackTraceElement caller) {
            return string;
        }
    }

    public static class DateLogPattern extends LogPattern {

        private final SimpleDateFormat dateFormat;

        @SuppressLint("SimpleDateFormat")
        public DateLogPattern(int count, int length, String dateFormat) {
            super(count, length);
            if (dateFormat != null) {
                this.dateFormat = new SimpleDateFormat(dateFormat);
            } else {
                this.dateFormat = new SimpleDateFormat("HH:mm:ss.SSS");
            }

        }

        @Override
        protected String doApply(StackTraceElement caller) {
            return dateFormat.format(new Date());
        }

    }

    public static class CallerLogPattern extends LogPattern {

        private int callerCount;
        private int callerLength;

        public CallerLogPattern(int count, int length, int callerCount, int callerLength) {
            super(count, length);
            this.callerCount = callerCount;
            this.callerLength = callerLength;
        }

        @Override
        protected String doApply(StackTraceElement caller) {
            if (caller == null) {
                throw new IllegalArgumentException("Caller not found");
            } else {
                String callerString;
                if (caller.getLineNumber() < 0) {
                    callerString = String.format("%s#%s", caller.getClassName(), caller
                            .getMethodName());
                } else {
                    String stackTrace = caller.toString();
                    stackTrace = stackTrace.substring(stackTrace.lastIndexOf('('), stackTrace
                            .length());
                    callerString = String.format("%s.%s%s", caller.getClassName(), caller
                            .getMethodName(), stackTrace);
                }
                try {
                    return LogConvert.shortenClassName(callerString, callerCount, callerLength);
                } catch (Exception e) {
                    return e.getMessage();
                }
            }
        }

        @Override
        protected boolean isCallerNeeded() {
            return true;
        }
    }

    public static class ConcatenateLogPattern extends LogPattern {

        private final List<LogPattern> patternList;

        public ConcatenateLogPattern(int count, int length, List<LogPattern> patternList) {
            super(count, length);
            this.patternList = new ArrayList<>(patternList);
        }

        public void addPattern(LogPattern pattern) {
            patternList.add(pattern);
        }

        @Override
        protected String doApply(StackTraceElement caller) {
            StringBuilder builder = new StringBuilder();
            for (LogPattern pattern : patternList) {
                builder.append(pattern.apply(caller));
            }
            return builder.toString();
        }

        @Override
        protected boolean isCallerNeeded() {
            for (LogPattern pattern : patternList) {
                if (pattern.isCallerNeeded()) {
                    return true;
                }
            }
            return false;
        }
    }

    public static class ThreadNameLogPattern extends LogPattern {
        public ThreadNameLogPattern(int count, int length) {
            super(count, length);
        }

        @Override
        protected String doApply(StackTraceElement caller) {
            return Thread.currentThread().getName();
        }
    }

    public static class Compiler {

        private String patternString;
        private int position;
        private List<ConcatenateLogPattern> queue;

        private final Pattern PERCENT_PATTERN = Pattern.compile("%%");
        private final Pattern NEWLINE_PATTERN = Pattern.compile("%n");
        private final Pattern CALLER_PATTERN = Pattern.compile("%([+-]?\\d+)?(\\.([+-]?\\d+))" +
                "?caller(\\{([+-]?\\d+)?(\\.([+-]?\\d+))?\\})?");
        private final Pattern DATE_PATTERN = Pattern.compile("%date(\\{(.*?)\\})?");
        private final Pattern CONCATENATE_PATTERN = Pattern.compile("%([+-]?\\d+)?(\\.([+-]?\\d+)" +
                ")?\\(");
        private final Pattern DATE_PATTERN_SHORT = Pattern.compile("%d(\\{(.*?)\\})?");
        private final Pattern CALLER_PATTERN_SHORT = Pattern.compile("%([+-]?\\d+)?(\\." +
                "([+-]?\\d+))?c(\\{([+-]?\\d+)?(\\.([+-]?\\d+))?\\})?");
        private final Pattern THREAD_NAME_PATTERN = Pattern.compile("%([+-]?\\d+)?(\\.([+-]?\\d+)" +
                ")?thread");
        private final Pattern THREAD_NAME_PATTERN_SHORT = Pattern.compile("%([+-]?\\d+)?(\\." +
                "([+-]?\\d+))?t");

        public LogPattern compile(String string) {
            if (string == null) {
                return null;
            }
            this.position = 0;
            this.patternString = string;
            this.queue = new ArrayList<>();
            queue.add(new ConcatenateLogPattern(0, 0, new ArrayList<LogPattern>()));
            while (string.length() > position) {
                int index = string.indexOf("%", position);
                int bracketIndex = string.indexOf(")", position);
                if (queue.size() > 1 && bracketIndex < index) {
                    queue.get(queue.size() - 1).addPattern(new PlainLogPattern(0, 0, string
                            .substring(position, bracketIndex)));
                    queue.get(queue.size() - 2).addPattern(queue.remove(queue.size() - 1));
                    position = bracketIndex + 1;
                }
                if (index == -1) {
                    queue.get(queue.size() - 1).addPattern(new PlainLogPattern(0, 0, string
                            .substring(position)));
                    break;
                } else {
                    queue.get(queue.size() - 1).addPattern(new PlainLogPattern(0, 0, string
                            .substring(position, index)));
                    position = index;
                    parse();
                }
            }
            return queue.get(0);
        }

        private void parse() {
            Matcher matcher;
            if ((matcher = findPattern(PERCENT_PATTERN)) != null) {
                queue.get(queue.size() - 1).addPattern(new PlainLogPattern(0, 0, "%"));
                position = matcher.end();
                return;
            }
            if ((matcher = findPattern(NEWLINE_PATTERN)) != null) {
                queue.get(queue.size() - 1).addPattern(new PlainLogPattern(0, 0, "\n"));
                position = matcher.end();
                return;
            }
            // the order is important because short logger pattern may match long caller occurrence
            if ((matcher = findPattern(CALLER_PATTERN)) != null || (matcher = findPattern
                    (CALLER_PATTERN_SHORT)) != null) {
                int count = Integer.parseInt(matcher.group(1) == null ? "0" : matcher.group(1));
                int length = Integer.parseInt(matcher.group(3) == null ? "0" : matcher.group(3));
                int countCaller = Integer.parseInt(matcher.group(5) == null ? "0" : matcher.group
                        (5));
                int lengthCaller = Integer.parseInt(matcher.group(7) == null ? "0" : matcher
                        .group(7));
                queue.get(queue.size() - 1).addPattern(new CallerLogPattern(count, length,
                        countCaller, lengthCaller));
                position = matcher.end();
                return;
            }
            if ((matcher = findPattern(DATE_PATTERN)) != null || (matcher = findPattern
                    (DATE_PATTERN_SHORT)) != null) {
                String dateFormat = matcher.group(2);
                queue.get(queue.size() - 1).addPattern(new DateLogPattern(0, 0, dateFormat));
                position = matcher.end();
                return;
            }
            if ((matcher = findPattern(THREAD_NAME_PATTERN)) != null || (matcher = findPattern
                    (THREAD_NAME_PATTERN_SHORT)) != null) {
                int count = Integer.parseInt(matcher.group(1) == null ? "0" : matcher.group(1));
                int length = Integer.parseInt(matcher.group(3) == null ? "0" : matcher.group(3));
                queue.get(queue.size() - 1).addPattern(new ThreadNameLogPattern(count, length));
                position = matcher.end();
                return;
            }
            if ((matcher = findPattern(CONCATENATE_PATTERN)) != null) {
                int count = Integer.parseInt(matcher.group(1) == null ? "0" : matcher.group(1));
                int length = Integer.parseInt(matcher.group(3) == null ? "0" : matcher.group(3));
                queue.add(new ConcatenateLogPattern(count, length, new ArrayList<LogPattern>()));
                position = matcher.end();
                return;
            }

            throw new IllegalArgumentException();
        }

        private Matcher findPattern(Pattern pattern) {
            Matcher matcher = pattern.matcher(patternString);
            return matcher.find(position) && matcher.start() == position ? matcher : null;
        }

    }
}
