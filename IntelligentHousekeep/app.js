//app.js
App({
    onLaunch: function() {
      if (wx.cloud) {
        wx.cloud.init({
          traceUser: true
        })
      }
      wx.getSystemInfo({
        success: e => {
          this.globalData.StatusBar = e.statusBarHeight;
          let capsule = wx.getMenuButtonBoundingClientRect();
          if (capsule) {
               this.globalData.Custom = capsule;
              this.globalData.CustomBar = capsule.bottom + capsule.top - e.statusBarHeight;
          } else {
              this.globalData.CustomBar = e.statusBarHeight + 50;
          }
        }
      });
    },
    globalData: {
      MQTTUid:"63d67cb328a3480d80d18e4cc8c41002",
      ColorList: [{
          title: '嫣红',
          name: 'red',
          color: '#e54d42'
        },
        {
          title: '桔橙',
          name: 'orange',
          color: '#f37b1d'
        },
        {
          title: '明黄',
          name: 'yellow',
          color: '#fbbd08'
        },
        {
          title: '橄榄',
          name: 'olive',
          color: '#8dc63f'
        },
        {
          title: '森绿',
          name: 'green',
          color: '#39b54a'
        },
        {
          title: '天青',
          name: 'cyan',
          color: '#1cbbb4'
        },
        {
          title: '海蓝',
          name: 'blue',
          color: '#0081ff'
        },
        {
          title: '姹紫',
          name: 'purple',
          color: '#6739b6'
        },
        {
          title: '木槿',
          name: 'mauve',
          color: '#9c26b0'
        },
        {
          title: '桃粉',
          name: 'pink',
          color: '#e03997'
        },
        {
          title: '棕褐',
          name: 'brown',
          color: '#a5673f'
        },
        {
          title: '玄灰',
          name: 'grey',
          color: '#8799a3'
        },
        {
          title: '草灰',
          name: 'gray',
          color: '#aaaaaa'
        },
        {
          title: '墨黑',
          name: 'black',
          color: '#333333'
        },
        {
          title: '雅白',
          name: 'white',
          color: '#ffffff'
        },
      ]
    },
    //获取所有主题
    getAlltopic_MQTT:function(fun){
        var that=this;
        var url="https://apis.bemfa.com/va/alltopic";
        this.getReq(url,{uid:this.globalData.MQTTUid,type:1})
        .then((res) => {
            console.log(res);
            // 处理返回的数据
            if(fun!=undefined){
                fun(res);
            }
        })
        .catch((err) => {
            console.error(err);
            // 处理请求异常
        });
    },
    //推送消息
    sendPostJsonMsg_MQTT:function(topic,msgContent,fun){
        var that=this;
        var url="https://apis.bemfa.com/va/postJsonMsg";
        this.postReq(url,{uid:this.globalData.MQTTUid,topic:topic,msg:msgContent,type:1})
        .then((res) => {
            console.log(res);
            // 处理返回的数据
            if(fun!=undefined){
                fun(res);
            }
        })
        .catch((err) => {
            console.error(err);
            // 处理请求异常
        });
    },
    // 封装GET请求
    getReq:function(url, params) {
        return new Promise((resolve, reject) => {
        wx.request({
            url: url,
            method: 'GET',
            data: params,
            success: (res) => {
            resolve(res.data);
            },
            fail: (err) => {
            reject(err);
            },
        });
        });
    },    
    // 封装POST请求
    postReq:function(url, params) {
        return new Promise((resolve, reject) => {
        wx.request({
            url: url,
            method: 'POST',
            data: params,
            success: (res) => {
            resolve(res.data);
            },
            fail: (err) => {
            reject(err);
            },
        });
        });
    }
  })