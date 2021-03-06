type position =
  | Title
  | Subtitle
  | Text
  | Nav;

module Styles = {
  open Css;

  let container = style([display(`flex), cursor(`pointer)]);

  let clickable = active =>
    active ? style([pointerEvents(`auto)]) : style([pointerEvents(`none)]);

  let prefix = style([fontWeight(`num(600))]);

  let font =
    fun
    | Title =>
      style([
        fontSize(`px(18)),
        lineHeight(`px(24)),
        Media.mobile([fontSize(px(14)), lineHeight(`px(18))]),
      ])
    | Subtitle =>
      style([
        fontSize(`px(14)),
        lineHeight(`px(20)),
        letterSpacing(`em(0.02)),
        Media.mobile([fontSize(`px(12))]),
      ])
    | Text => style([fontSize(`px(12)), lineHeight(`px(16))])
    | Nav => style([fontSize(`px(10)), lineHeight(`px(14))]);

  let base =
    style([
      overflow(`hidden),
      textOverflow(`ellipsis),
      whiteSpace(`nowrap),
      display(`block),
      color(Colors.gray7),
      hover([color(Colors.gray7)]),
      active([color(Colors.gray7)]),
    ]);

  let wordBreak =
    style([Media.mobile([textOverflow(`unset), whiteSpace(`unset), wordBreak(`breakAll)])]);

  let copy = style([width(`px(15)), marginLeft(`px(10)), cursor(`pointer)]);

  let setWidth =
    fun
    | Title => style([Media.mobile([width(`percent(90.))])])
    | _ => "";
};

[@react.component]
let make =
    (
      ~address,
      ~position=Text,
      ~accountType=`account,
      ~copy=false,
      ~clickable=true,
      ~wordBreak=false,
    ) => {
  let isValidator = accountType == `validator;
  let prefix = isValidator ? "bandvaloper" : "band";

  let noPrefixAddress =
    isValidator
      ? address |> Address.toOperatorBech32 |> Js.String.sliceToEnd(~from=11)
      : address |> Address.toBech32 |> Js.String.sliceToEnd(~from=4);

  <>
    <Link
      className={Css.merge([
        Styles.container,
        Styles.clickable(clickable),
        Styles.setWidth(position),
      ])}
      route={
        isValidator
          ? Route.ValidatorIndexPage(address, Route.ProposedBlocks)
          : Route.AccountIndexPage(address, Route.AccountTransactions)
      }>
      <span
        className={Css.merge([
          Styles.base,
          Text.Styles.code,
          Styles.font(position),
          wordBreak ? Styles.wordBreak : "",
        ])}>
        <span className=Styles.prefix> {prefix |> React.string} </span>
        {noPrefixAddress |> React.string}
      </span>
    </Link>
    {copy
       ? <>
           {switch (position) {
            | Title => <HSpacing size=Spacing.md />
            | _ => <HSpacing size=Spacing.sm />
            }}
           <CopyRender
             width={
               switch (position) {
               | Title => 15
               | _ => 12
               }
             }
             message={
               isValidator ? address |> Address.toOperatorBech32 : address |> Address.toBech32
             }
           />
         </>
       : React.null}
  </>;
};
