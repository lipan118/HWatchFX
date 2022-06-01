package com.wiyixiao.hwatchfx.core;

import java.util.Comparator;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.PriorityBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

public class PriorityExecutor extends ThreadPoolExecutor {

    /**
     * @Desc: 核心线程池大小
     */
    private static final int CORE_POOL_SIZE = 8;

    /**
     * @Desc: 最大线程池队列大小
     */
    private static final int MAXIMUM_POOL_SIZE = 256;

    /**
     * @Desc: 保持存活时间，当线程数大于corePoolSize的空闲线程能保持的最大时间
     */
    private static final int KEEP_ALIVE = 1;
    
    /**
     * @Desc: 主要获取添加任务
     */
    private static final AtomicLong SEQ_SEED = new AtomicLong(0);

    /**
     * 创建线程工厂
     */
    private static final ThreadFactory sThreadFactory = new ThreadFactory() {
        private final AtomicInteger mCount = new AtomicInteger(1);

        @Override
        public Thread newThread(Runnable runnable) {
            return new Thread(runnable, "LinkZill-TP#" + mCount.getAndIncrement());
        }
    };


    /**
     * 线程队列方式 先进先出
     */
    private static final Comparator<Runnable> FIFO = new Comparator<Runnable>() {
        @Override
        public int compare(Runnable lhs, Runnable rhs) {
            if (lhs instanceof PriorityRunnable && rhs instanceof PriorityRunnable) {
                PriorityRunnable lpr = ((PriorityRunnable) lhs);
                PriorityRunnable rpr = ((PriorityRunnable) rhs);
                int result = lpr.priority - rpr.priority;
                return result == 0 ? (int) (lpr.SEQ - rpr.SEQ) : result;
            } else {
                return 0;
            }
        }
    };

    /**
     * 线程队列方式 后进先出
     */
    private static final Comparator<Runnable> LIFO = new Comparator<Runnable>() {
        @Override
        public int compare(Runnable lhs, Runnable rhs) {
            if (lhs instanceof PriorityRunnable && rhs instanceof PriorityRunnable) {
                PriorityRunnable lpr = ((PriorityRunnable) lhs);
                PriorityRunnable rpr = ((PriorityRunnable) rhs);
                int result = lpr.priority - rpr.priority;
                return result == 0 ? (int) (rpr.SEQ - lpr.SEQ) : result;
            } else {
                return 0;
            }
        }
    };

    /**
     * 默认工作线程数5
     *
     * @param fifo 优先级相同时, 等待队列的是否优先执行先加入的任务.
     */
    public PriorityExecutor(boolean fifo) {
        this(CORE_POOL_SIZE, fifo);
    }

    /**
     * @param poolSize 工作线程数
     * @param fifo     优先级相同时, 等待队列的是否优先执行先加入的任务.
     */
    public PriorityExecutor(int poolSize, boolean fifo) {
        this(poolSize, MAXIMUM_POOL_SIZE, KEEP_ALIVE, TimeUnit.SECONDS, new PriorityBlockingQueue<Runnable>(MAXIMUM_POOL_SIZE, fifo ? FIFO : LIFO), sThreadFactory);
    }

    public PriorityExecutor(int corePoolSize, int maximumPoolSize, long keepAliveTime, TimeUnit unit, BlockingQueue<Runnable> workQueue, ThreadFactory threadFactory) {
        super(corePoolSize, maximumPoolSize, keepAliveTime, unit, workQueue, threadFactory);
    }

    /**
     * 判断当前线程池是否繁忙
     * @return
     */
    public boolean isBusy() {
        return getActiveCount() >= getCorePoolSize();
    }

    /**
     * 提交任务
     * @param runnable
     */
    @Override
    public void execute(Runnable runnable) {
        if (runnable instanceof PriorityRunnable) {
            ((PriorityRunnable) runnable).SEQ = SEQ_SEED.getAndIncrement();
        }
        super.execute(runnable);
    }
}
