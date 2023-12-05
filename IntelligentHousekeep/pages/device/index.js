const app=getApp();
Page({

    /**
     * 页面的初始数据
     */
    data: {
        topic:"",//设备的MQTT主题
        title:"设备管理",
        ledSwitch:false,//灯开头状态
    },

    /**
     * 生命周期函数--监听页面加载
     */
    onLoad(options) {
        console.log(options)
        this.setData({title:options.title,topic:options.topic});
    },
    changeLedSwitch(){//切换开头状态
        var ledSwitch=this.data.ledSwitch;
        this.setData({ledSwitch:ledSwitch?false:true});
        
        var sendData={
            cmdName:"switchLed",
            cmdVal:{
                "switchLedState":this.data.ledSwitch?1:0
            }
        };
        //向设备发送指令
        var that=this;
        app.sendPostJsonMsg_MQTT(this.data.topic,JSON.stringify(sendData),function(res){
            if(res.code==0){
                wx.showToast({
                  title: '操作成功',
                })
            }else{
                that.setData({ledSwitch:that.data.ledSwitch?false:true});
                wx.showModal({
                  title: '温馨提示',
                  content: res.message,
                  showCancel:false,
                  complete: (res) => {
                      console.log(res)
                  }
                })
            }
        });
    },
    /**
     * 生命周期函数--监听页面初次渲染完成
     */
    onReady() {

    },

    /**
     * 生命周期函数--监听页面显示
     */
    onShow() {

    },

    /**
     * 生命周期函数--监听页面隐藏
     */
    onHide() {

    },

    /**
     * 生命周期函数--监听页面卸载
     */
    onUnload() {

    },

    /**
     * 页面相关事件处理函数--监听用户下拉动作
     */
    onPullDownRefresh() {

    },

    /**
     * 页面上拉触底事件的处理函数
     */
    onReachBottom() {

    },

    /**
     * 用户点击右上角分享
     */
    onShareAppMessage() {

    }
})