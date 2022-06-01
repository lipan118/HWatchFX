package com.wiyixiao.hwatchfx.core;

import androidx.annotation.IntDef;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

@IntDef({PriorityType.HIGH, PriorityType.NORMAL,PriorityType.LOW})
@Retention(RetentionPolicy.SOURCE)
public @interface PriorityType {

    /**
     * @Desc: 不设置优先级，默认正常
     */
    int NONE = -1;

    /**
     * @Desc: 优先级高
     */
    int HIGH = 0;

    /**
     * @Desc: 优先级正常
     */
    int NORMAL = 1;

    /**
     * @Desc: 优先级低
     */
    int LOW = 2;

}
