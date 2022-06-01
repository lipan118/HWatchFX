package com.zyao89.view.zloadingL;

import com.zyao89.view.zloadingL.ball.ElasticBallBuilder;
import com.zyao89.view.zloadingL.ball.InfectionBallBuilder;
import com.zyao89.view.zloadingL.ball.IntertwineBuilder;
import com.zyao89.view.zloadingL.circle.DoubleCircleBuilder;
import com.zyao89.view.zloadingL.circle.PacManBuilder;
import com.zyao89.view.zloadingL.circle.RotateCircleBuilder;
import com.zyao89.view.zloadingL.circle.SingleCircleBuilder;
import com.zyao89.view.zloadingL.circle.SnakeCircleBuilder;
import com.zyao89.view.zloadingL.clock.CircleBuilder;
import com.zyao89.view.zloadingL.clock.ClockBuilder;
import com.zyao89.view.zloadingL.path.MusicPathBuilder;
import com.zyao89.view.zloadingL.path.SearchPathBuilder;
import com.zyao89.view.zloadingL.path.StairsPathBuilder;
import com.zyao89.view.zloadingL.rect.ChartRectBuilder;
import com.zyao89.view.zloadingL.rect.StairsRectBuilder;
import com.zyao89.view.zloadingL.star.LeafBuilder;
import com.zyao89.view.zloadingL.star.StarBuilder;
import com.zyao89.view.zloadingL.text.TextBuilder;

/**
 * Created by zyao89 on 2017/3/19.
 * Contact me at 305161066@qq.com or zyao89@gmail.com
 * For more projects: https://github.com/zyao89
 * My Blog: http://zyao89.me
 */
public enum Z_TYPE
{
    CIRCLE(CircleBuilder.class),
    CIRCLE_CLOCK(ClockBuilder.class),
    STAR_LOADING(StarBuilder.class),
    LEAF_ROTATE(LeafBuilder.class),
    DOUBLE_CIRCLE(DoubleCircleBuilder.class),
    PAC_MAN(PacManBuilder.class),
    ELASTIC_BALL(ElasticBallBuilder.class),
    INFECTION_BALL(InfectionBallBuilder.class),
    INTERTWINE(IntertwineBuilder.class),
    TEXT(TextBuilder.class),
    SEARCH_PATH(SearchPathBuilder.class),
    ROTATE_CIRCLE(RotateCircleBuilder.class),
    SINGLE_CIRCLE(SingleCircleBuilder.class),
    SNAKE_CIRCLE(SnakeCircleBuilder.class),
    STAIRS_PATH(StairsPathBuilder.class),
    MUSIC_PATH(MusicPathBuilder.class),
    STAIRS_RECT(StairsRectBuilder.class),
    CHART_RECT(ChartRectBuilder.class),;

    private final Class<?> mBuilderClass;

    Z_TYPE(Class<?> builderClass)
    {
        this.mBuilderClass = builderClass;
    }

    <T extends ZLoadingBuilder> T newInstance()
    {
        try
        {
            return (T) mBuilderClass.newInstance();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return null;
    }
}
