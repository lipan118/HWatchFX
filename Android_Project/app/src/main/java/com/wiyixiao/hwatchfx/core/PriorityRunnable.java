package com.wiyixiao.hwatchfx.core;

public class PriorityRunnable implements Runnable{
    /**
     * @Desc: 任务优先级
     */
    public final @PriorityType int priority;

    /**
     * @Desc: 任务真正执行者
     */
    private final Runnable runnable;

    /**
     * @Desc: 任务唯一标示
     */
    long SEQ;

    public PriorityRunnable(@PriorityType int priority, Runnable runnable) {
        this.priority = (priority == PriorityType.NONE) ? PriorityType.NORMAL : priority;
        this.runnable = runnable;
    }

    @Override
    public final void run() {
        this.runnable.run();
    }
}
