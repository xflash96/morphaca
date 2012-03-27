var app = (function ($){
	var Router = Backbone.Router.extend({
        el: null,
        initialize: function(el) {
            this.el = el;

            this.mainView = new MainView();
        },
		routes: {
            "": "main",
            "upload": "upload",
            "morph/:id": "morphDetails",
            "report": "report",
            "contact": "contact",
        },
        main: function() {
            this.mainView = new MainView();
            this.mainView.render();
        },
        upload: function() {
            this.uploadView = new UploadView();
            this.uploadView.render();
        },
        morphDetails: function(id) {
            this.morph = new Morph({id: id});
        },
        contact: function() {
            this.contactView = new ContactView();
            this.contactView.render();
        },
        report: function() {
            this.reportView = new ReportView();
            this.reportView.render();
        }
	});

    window.Morph = Backbone.Model.extend({urlRoot: '/morph'});
    window.MorphCollection = Backbone.Collection.extend({
        model: Morph,
        url: "post",
    });

    var ShowCaseView = Backbone.View.extend({
        tmplHtml: $('#showcase-tmpl').html(),
        template: null,
        initialize: function() {
            this.template = Handlebars.compile(this.tmplHtml);
        },
        render: function() {
            html = this.template({photos: this.model.models, blockId: this.blockId});
            return html;
        },
    });

    var Photo = Backbone.Model.extend();

    var zeropad = function(v,l){
        t = v.toString();
        while(t.length < l){
            t = '0'+t;
        }
        return t;
    };

    var MainView = Backbone.View.extend({
        tmplHtml: $('#main-tmpl').html(),
        template: null,
        el: $('#content'),
        initialize: function(){
            this.template = Handlebars.compile(this.tmplHtml);
            this.photoList = [];
            for (var i=0; i<4; i++){
                n = new Object;
                n.url = '/static/img/sample/test'+zeropad(i*2, 2)+'.jpg';
                n.msg = '<h3>( '+(i+1)+' )</h3>';
                this.photoList.push(n);
            }
            console.log(this.photoList);
            this.showcaseView = new ShowCaseView({
                model: {models: this.photoList}
                });
            this.showcaseView.blockId = 'mainShowcase';
            //this.model.bind('change', this.render, this);
            //this.model.bind('destroy', this.remove, this);
        },
        render: function(){
            showcaseHtml = this.showcaseView.render();
            html = this.template();
            $(this.el).html(html);
            $('#mainShowcase').html(showcaseHtml)
                .ready(function(){
                    $('.carousel').carousel({interval:500});
                })
        },
    });

    var UploadView = Backbone.View.extend({
        tmplHtml: $('#upload-tmpl').html(),
        el: $('#content'),
        events: {
            'change #srcImage': function(e) {
                    this.preview(e, $('#srcImagePreview'), '<h2>From</h2>'); 
                },
            'change #dstImage': function(e) {
                    this.preview(e, $('#dstImagePreview'), '<h2>To</h2>');
                },
        },
        initialize: function(){
            this.template = Handlebars.compile(this.tmplHtml);
        },
        render: function(){
            var context = {
                'alertStyle': 'alert alert-info',
                'msg': 'Upload two image, and we will morph them. Cool?',
            };
            html = this.template(context);
            $(this.el).html(html);
        },
        preview: function(e, result, msg) {
            window.loadImage(
                e.target.files[0],
                function (img) {
                    window.tmpimg = img;
                    result.html(img);
                    result.prepend(msg);
                },
                {maxWidth: window.innerWidth * 0.35,
                 maxHeight: window.innerHeight * 0.35,
                 canvas: true}
            );
        },
    });

    var MorphDetailView = Backbone.View.extend({
        tmplHtml: $('#detail-tmpl').html(),
        el: $('#content'),
        initialize: function(){
            this.template = Handlebars.compile(this.tmplHtml);
        },
        render: function(){
            var context = {};
            html = this.template(context);
            $(this.el).html(html);
        }
    });

    var CreoleView = Backbone.View.extend({
        el: null,
        creole: null,
        options: {},
        initialize: function(){
            this.creole = new Parse.Simple.Creole({
                forIE: document.all,
            });
        },
        render: function(div, markup){
                this.creole.parse(div, markup);
                //MathJax.Hub.Queue(['Typeset', MathJax.Hub, div]);
        },
    });

    var ContactView = Backbone.View.extend({
        creoleView: null,
        tmplHtml: $('#contact-tmpl').html(),
        el: $('#content'),
        initialize: function(){
            this.template = Handlebars.compile(this.tmplHtml);
            this.creoleView = new CreoleView();
        },
        render: function(){
            var context = {};
            html = this.template(context);
            $(this.el).html(html);
        },
    });

    var ReportView = Backbone.View.extend({
        creoleView: null,
        tmplHtml: $('#report-tmpl').html(),
        el: $('#content'),
        initialize: function(){
            this.template = Handlebars.compile(this.tmplHtml);
            this.creoleView = new CreoleView();
        },
        render: function(){
            var context = {};
            html = this.template(context);
            var callback = function(){
                $.get('report/report.txt', this.getMarkup);
            }
            _.bindAll(this, 'getMarkup');
            callback = _.bind(callback, this);
            $(this.el).html(html).ready(callback);
        },
        getMarkup: function(data){
            var div = $('#wiki')[0];
            this.creoleView.render(div, data);
            prettyPrint();
            LatexIT.render('span');
        },
    });

    $(document).ready(function(){
        var content = $('#content');
        var app = new Router(content);
        Backbone.history.start();
    });

    return app;
})(jQuery);
