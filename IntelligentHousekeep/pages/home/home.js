// pages/home/home.js
const app=getApp();
Page({

    /**
     * 页面的初始数据
     */
    data: {
        elements: [
            // { title: 'ESP32', name: 'bar', color: 'purple', icon: 'vipcard' },
            // { title: '导航栏 ', name: 'nav', color: 'mauve', icon: 'formfill' },
            // { title: '列表', name: 'list', color: 'pink', icon: 'list' },
          ],
    },

    /**
     * 生命周期函数--监听页面加载
     */
    onLoad(options) {
        this.selectComponent("#bottomBtnList").setData({PageCur:"basics"});

        var that=this;
        app.getAlltopic_MQTT(function(res){
            var list=res.data;
            if(list.length>0){
                for(var i=0;i<list.length;i++){
                    var item=list[i];
                    item.title=item.topic;
                    item.name=item.online?"在线":"离线";
                    item.color= 'purple';
                    item.icon='vipcard';
                }
            }
            that.setData({elements:list});
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
        console.log(333333333)
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